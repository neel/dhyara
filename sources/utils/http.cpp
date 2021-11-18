/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/utils/http.h"
#include <sstream>
#include "dhyara/assets.h"
#include <cstring>
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_wifi.h"
#include <dhyara/dhyara.h>
#include "esp_idf_version.h"

#if CONFIG_ENABLE_SERVICES_OVER_HTTP

#include "dhyara/detail/args_helper.hpp"
#include "dhyara/services/ping.h"
#include "dhyara/services/identify.h"
#include "dhyara/services/routes.h"
#include "dhyara/services/universe.h"
#include "dhyara/services/phy_rate.h"

# endif

namespace fragments{

static const char sidebar[] = 
    "<div class='sidebar'>"
        "<div class='logo'>Dhyara</div>"
        "<ul id='menu' class='contents'>"
            "<li data-label='Home' class='home' data-href='/' />"
            "<li data-label='Routing' class='routing' data-href='/routing' />"
            "<li data-label='Peers' class='neighbourhood' data-href='/peers' />"
        "</ul>"
    "</div>";
    
static const char banner[] = 
    "<div class='full-w'>"
        "<div class='board banner'>"
            "<div id='info-iface' class='groups'>"
                "<div class='group'>"
                    "<div class='cell' data-key='ssid'></div>"
                    "<div class='cell' data-key='mac'></div>"
                "</div>"
                "<div class='group'>"
                    "<div class='cell' data-key='channel'></div>"
                    "<div class='cell' data-key='frequency'></div>"
                "</div>"
                "<div class='group'>"
                    "<div class='cell' data-key='power saving'></div>"
                    "<div class='cell' data-key='max power'></div>"
                "</div>"
                "<div class='group'>"
                    "<div class='cell' data-key='authentication'></div>"
                    "<div class='cell' data-key='max clients'></div>"
                "</div>"
            "</div>"
        "</div>"
    "</div>";

static const char home[] = 
    "<div class='full-flex-hz'>"
        "<div class='board cluster'>"
            "<div class='heading'>Dhyara Configuration</div>"
            "<div class='groups' id='info-dhyara'>"
                "<div class='group'>"
                    "<div class='cell' data-key='send queueing'></div>"
                    "<div class='cell' data-key='queue size'></div>"
                    "<div class='cell' data-key='sync queue size'></div>"
                    "<div class='cell' data-key='broadcast channel'></div>"
                    "<div class='cell' data-key='peer channel'></div>"
                "</div>"
                "<div class='group'>"
                    "<div class='cell' data-key='beacon interval'></div>"
                    "<div class='cell' data-key='delay tolerance'></div>"
                    "<div class='cell' data-key='depreciation coefficient'></div>"
                    "<div class='cell' data-key='advertisement expiry'></div>"
                    "<div class='cell' data-key='route expiry'></div>"
                "</div>"
            "</div>"
        "</div>"
        "<div class='board cluster'>"
            "<div class='heading'>Counters</div>"
            "<div class='groups' id='counter'>"
                "<div class='group'>"
                    "<div class='cell' data-key='beacon'><span class='value' /></div>"
                    "<div class='cell' data-key='acknowledgement'><span class='value' /></div>"
                    "<div class='cell' data-key='advertisement'><span class='value' /></div>"
                    "<div class='cell' data-key='chunk'><span class='value' /></div>"
                    "<div class='cell' data-key='delivered'><span class='value' /></div>"
                "</div>"
                "<div class='group'>"
                    "<div class='cell' data-key='echo request'><span class='value' /></div>"
                    "<div class='cell' data-key='echo reply'><span class='value' /></div>"
                    "<div class='cell' data-key='echo lost'><span class='value' /></div>"
                "</div>"
            "</div>"
        "</div>"
    "</div>";
    
static const char routing[] = 
    "<div class='full-flex-hz'>"
        "<div class='board cluster-full'>"
            "<div class='heading'>Routing Table</div>"
            "<table>"
                "<thead>"
                    "<tr>"
                        "<th>Destination</th>"
                        "<th>Intermediate</th>"
                        "<th>Delay</th>"
                        "<th>Hops</th>"
                        "<th>Updated</th>"
                    "</tr>"
                "</thead>"
                "<tbody id='routes-body'></tbody>"
            "</table>"
        "</div>"
    "</div>"
    "<div class='full-flex-hz'>"
        "<div class='board cluster-full'>"
            "<div class='heading'>Best Route</div>"
            "<table>"
                "<thead>"
                    "<tr>"
                        "<th>Destination</th>"
                        "<th>Intermediate</th>"
                        "<th>Delay</th>"
                        "<th>Hops</th>"
                    "</tr>"
                "</thead>"
                "<tbody id='next-body'></tbody>"
            "</table>"
        "</div>"
    "</div>";
    
static const char peers[] = 
    "<div class='full-flex-hz'>"
        "<div class='board cluster-full'>"
            "<div class='heading'>Neighbours</div>"
            "<table>"
                "<thead>"
                    "<tr>"
                        "<th>Address</th>"
                        "<th>Name</th>"
                        "<th>Channel</th>"
                        "<th>RSSI</th>"
                    "</tr>"
                "</thead>"
                "<tbody id='neighbours-body'></tbody>"
            "</table>"
        "</div>"
    "</div>"
    "<div class='full-flex-hz'>"
        "<div class='board cluster-full'>"
            "<div class='heading'>Universe</div>"
            "<table>"
                "<thead>"
                    "<tr>"
                        "<th>Address</th>"
                        "<th>Name</th>"
                    "</tr>"
                "</thead>"
                "<tbody id='universe-body'></tbody>"
            "</table>"
        "</div>"
    "</div>";

}

static const char* html_open = 
    "<!DOCTYPE html>" 
    "<html>";
static const char* wrapper_open = 
        "<body>" 
            "<div class='wrapper'>";
static const char* central_open = 
                "<div class='main'>"
                    "<div class='central'>";
static const char* central_close = 
                    "</div>" 
                "</div>";
// wrapper close will be different for each html pages, so defined inside respective functions
static const char* html_close = 
    "</html>";

dhyara::utils::http::http(dhyara::link& link): _link(link), _config(HTTPD_DEFAULT_CONFIG()), _server(0x0)
    ,_index_html  (httpd_uri_t{"/",             HTTP_GET , dhyara::utils::http::index_html_handler,  this})
    ,_routes_html (httpd_uri_t{"/routing",      HTTP_GET , dhyara::utils::http::routes_html_handler, this})
    ,_peers_html  (httpd_uri_t{"/peers",        HTTP_GET , dhyara::utils::http::peers_html_handler,  this})
    ,_style       (httpd_uri_t{"/dhyara.css",   HTTP_GET , dhyara::utils::http::style_handler,       this})
    ,_icons       (httpd_uri_t{"/icons",        HTTP_GET , dhyara::utils::http::icons_handler,       this})
    ,_info        (httpd_uri_t{"/info.json",    HTTP_GET , dhyara::utils::http::info_handler,        this})
    ,_counter     (httpd_uri_t{"/counter.json", HTTP_GET , dhyara::utils::http::counter_handler,     this})
    ,_routes      (httpd_uri_t{"/routes.json",  HTTP_GET , dhyara::utils::http::routes_handler,      this})
    ,_peers       (httpd_uri_t{"/peers.json",   HTTP_GET , dhyara::utils::http::peers_handler,       this})
#if CONFIG_ENABLE_SERVICES_OVER_HTTP
    ,_command     (httpd_uri_t{"/command",      HTTP_POST, dhyara::utils::http::command_handler,     this})
#endif 
{
#if CONFIG_ENABLE_SERVICES_OVER_HTTP
    _config.max_uri_handlers = 11;
    _config.stack_size = 8192;
#else 
    _config.max_uri_handlers = 10;
#endif 
}


esp_err_t dhyara::utils::http::start(){
    esp_err_t res = httpd_start(&_server, &_config);
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_index_html);
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_routes_html);
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_peers_html);
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_style);
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_icons);
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_info);
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_counter);
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_routes);
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_peers);
#if CONFIG_ENABLE_SERVICES_OVER_HTTP
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_command);
#endif 
    return res;
}

esp_err_t dhyara::utils::http::index_html_handler(httpd_req_t* req){
    dhyara::utils::http* self = static_cast<dhyara::utils::http*>(req->user_ctx);
    return self->index_html(req);
}

esp_err_t dhyara::utils::http::style_handler(httpd_req_t* req){
    dhyara::utils::http* self = static_cast<dhyara::utils::http*>(req->user_ctx);
    return self->style(req);
}

esp_err_t dhyara::utils::http::peers_html_handler(httpd_req_t* req){
    dhyara::utils::http* self = static_cast<dhyara::utils::http*>(req->user_ctx);
    return self->peers_html(req);
}

esp_err_t dhyara::utils::http::icons_handler(httpd_req_t* req){
    dhyara::utils::http* self = static_cast<dhyara::utils::http*>(req->user_ctx);
    return self->icons(req);
}

esp_err_t dhyara::utils::http::info_handler(httpd_req_t* req){
    dhyara::utils::http* self = static_cast<dhyara::utils::http*>(req->user_ctx);
    return self->info(req);
}

esp_err_t dhyara::utils::http::counter_handler(httpd_req_t* req){
    dhyara::utils::http* self = static_cast<dhyara::utils::http*>(req->user_ctx);
    return self->counter(req);
}

esp_err_t dhyara::utils::http::routes_html_handler(httpd_req_t* req){
    dhyara::utils::http* self = static_cast<dhyara::utils::http*>(req->user_ctx);
    return self->routes_html(req);
}

esp_err_t dhyara::utils::http::routes_handler(httpd_req_t* req){
    dhyara::utils::http* self = static_cast<dhyara::utils::http*>(req->user_ctx);
    return self->routes(req);
}

esp_err_t dhyara::utils::http::peers_handler(httpd_req_t* req){
    dhyara::utils::http* self = static_cast<dhyara::utils::http*>(req->user_ctx);
    return self->peers(req);
}

#if CONFIG_ENABLE_SERVICES_OVER_HTTP

esp_err_t dhyara::utils::http::command_handler(httpd_req_t* req){
    dhyara::utils::http* self = static_cast<dhyara::utils::http*>(req->user_ctx);
    return self->command(req);
}

#endif 

esp_err_t dhyara::utils::http::index_html(httpd_req_t* req){
    static const char* wrapper_close = 
                "</div>" 
                "<script>"
                "setTimeout(() => {"
                    "fetch_banner();"
                    "fetch_dhyara();"
                    "poll_counters();"
                "}, 500);"
                "</script>"
            "</body>";
        
    httpd_resp_set_type(req, "text/html");
    httpd_resp_sendstr_chunk(req, html_open);
    httpd_resp_sendstr_chunk(req, (const char*)dhyara::assets::head_html_start);
    httpd_resp_sendstr_chunk(req, wrapper_open);
    httpd_resp_sendstr_chunk(req, fragments::sidebar);
    httpd_resp_sendstr_chunk(req, central_open);
    httpd_resp_sendstr_chunk(req, fragments::banner);
    httpd_resp_sendstr_chunk(req, fragments::home);
    httpd_resp_sendstr_chunk(req, central_close);
    httpd_resp_sendstr_chunk(req, wrapper_close);
    httpd_resp_sendstr_chunk(req, html_close);
    httpd_resp_sendstr_chunk(req, 0x0);
    return ESP_OK;
}

esp_err_t dhyara::utils::http::icons(httpd_req_t* req){
    const auto length = (dhyara::assets::icons_gif_end - dhyara::assets::icons_gif_start);
    httpd_resp_set_type(req, "image/gif");
    httpd_resp_send(req, (const char*)dhyara::assets::icons_gif_start, length);
    return ESP_OK;
}

esp_err_t dhyara::utils::http::style(httpd_req_t* req){
    const auto length = (dhyara::assets::dhyara_css_end - dhyara::assets::dhyara_css_start);
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char*)dhyara::assets::dhyara_css_start, length);
    return ESP_OK;
}

esp_err_t dhyara::utils::http::info(httpd_req_t* req){
    std::stringstream response_json;
    response_json << "{";
    
    wifi_config_t config;
    std::memset(&config, 0, sizeof(wifi_config_t));
    esp_err_t err = esp_wifi_get_config(WIFI_IF_AP, &config);
    if(err == ESP_OK){
        std::string ssid((const char*)config.ap.ssid, config.ap.ssid_len);
        
        std::uint8_t protocol;
        esp_wifi_get_protocol(WIFI_IF_AP, &protocol);
        wifi_bandwidth_t bandwidth;
        esp_wifi_get_bandwidth(WIFI_IF_AP, &bandwidth);
        
        std::string protocol_str;
        if(protocol & WIFI_PROTOCOL_11B) protocol_str += "b";
        if(protocol & WIFI_PROTOCOL_11G) protocol_str += "g";
        if(protocol & WIFI_PROTOCOL_11N) protocol_str += "n";
        if(protocol & WIFI_PROTOCOL_LR)  protocol_str += "lr";
        
        std::uint32_t channel_freq_begin = 2401 + 5*(config.ap.channel -1), 
                      channel_freq_end   = channel_freq_begin + ((bandwidth == WIFI_BW_HT20) ? 20 : 40),
                      channel_freq_mean  = (channel_freq_begin + channel_freq_end) / 2;
                      
        std::string auth_str;
        switch(config.ap.authmode){
            case WIFI_AUTH_OPEN:
                auth_str = "WIFI_AUTH_OPEN";
                break;
            case WIFI_AUTH_WEP:
                auth_str = "WIFI_AUTH_WEP";
                break;
            case WIFI_AUTH_WPA_PSK:
                auth_str = "WIFI_AUTH_WPA_PSK";
                break;
            case WIFI_AUTH_WPA2_PSK:
                auth_str = "WIFI_AUTH_WPA2_PSK";
                break;
            case WIFI_AUTH_WPA_WPA2_PSK:
                auth_str = "WIFI_AUTH_WPA_WPA2_PSK";
                break;
            case WIFI_AUTH_WPA2_ENTERPRISE:
                auth_str = "WIFI_AUTH_WPA2_ENTERPRISE";
                break;
            case WIFI_AUTH_WPA3_PSK:
                auth_str = "WIFI_AUTH_WPA3_PSK";
                break;
            case WIFI_AUTH_WPA2_WPA3_PSK:
                auth_str = "WIFI_AUTH_WPA2_WPA3_PSK";
                break;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 3, 1)
            case WIFI_AUTH_WAPI_PSK:
                auth_str = "WIFI_AUTH_WAPI_PSK";
                break;
#endif
            case WIFI_AUTH_MAX:
                auth_str = "WIFI_AUTH_MAX";
                break;
            default:
                auth_str = "Unknown";
        }
        
        std::stringstream ap_json;
        ap_json << "{";
        ap_json << "\"ssid\":" << '"' << ssid << '"' << ",";
        ap_json << "\"address\":" << '"' << dhyara_local().to_string() << '"' << ",";
        ap_json << "\"hidden\":" << std::boolalpha << (bool) config.ap.ssid_hidden << ",";
        ap_json << "\"channel\":" << (int)config.ap.channel << ",";
        ap_json << "\"frequency\":" << "{";
        ap_json <<      "\"begin\":" << channel_freq_begin << ",";
        ap_json <<      "\"end\":" << channel_freq_end << ",";
        ap_json <<      "\"mean\":" << channel_freq_mean;
        ap_json << "},";
        ap_json << "\"protocol\":" << '"' << "802.11" << protocol_str << '"' << ",";
        ap_json << "\"bandwidth\":" << ((bandwidth == WIFI_BW_HT20) ? 20 : 40) << ",";
        ap_json << "\"authentication\":" << '"' << auth_str << '"' << ",";
        ap_json << "\"max_connections\":" << (int)config.ap.max_connection;
        ap_json << "}";
        
        response_json << "\"ap\":" << ap_json.str();
    }
    
    response_json << ",";
    
    {
        wifi_ps_type_t type;
        err = esp_wifi_get_ps(&type);
        std::string ps_str;
        switch(type){
            case WIFI_PS_NONE:
                ps_str = "None";
                break;
            case WIFI_PS_MIN_MODEM:
                ps_str = "Min";
                break;
            case WIFI_PS_MAX_MODEM:
                ps_str = "Max";
                break;
            default:
                ps_str = "Unknown";
        }
        
        
        std::int8_t max_power;
        err = esp_wifi_get_max_tx_power(&max_power);
        
        std::stringstream power_json;
        power_json << "{";
        power_json << "\"saving\":" << '"' << ps_str << '"' << ",";
        power_json << "\"max\":" << (int) max_power;
        power_json << "}";
        
        response_json << "\"power\":" << power_json.str();
    }
    
    response_json << ",";
    
    {
        std::stringstream dhyara_json;
        dhyara_json << "{";
        dhyara_json << "\"send_queueing\":" << std::boolalpha << (bool)DHYARA_ENABLED_SEND_QUEUEING << ",";
        dhyara_json << "\"queue_size\":" << dhyara::queue_size << ",";
        dhyara_json << "\"sync_queue_size\":" << dhyara::sync_queue_size << ",";
        dhyara_json << "\"espnow_broadcast_channel\":" << (int) dhyara::espnow_broadcast_channel << ",";
        dhyara_json << "\"espnow_peer_channel\":" << (int) dhyara::espnow_peer_channel << ",";
        dhyara_json << "\"beacon_interval\":" << dhyara::beacon_interval << ",";
        dhyara_json << "\"delay_tolerance\":" << dhyara::delay_tolerance << ",";
        dhyara_json << "\"depreciation_coefficient\":" << (int) dhyara::depreciation_coefficient << ",";
        dhyara_json << "\"advertisement_expiry\":" << (dhyara::advertisement_expiry / 1000) << ",";
        dhyara_json << "\"route_expiry\":" << (dhyara::route_expiry / 1000);
        dhyara_json << "}";
        
        response_json << "\"dhyara\":" << dhyara_json.str();
    }
    
    response_json << "}";
    
    std::string response = response_json.str();
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response.c_str(), response.length());
    return ESP_OK;
}

esp_err_t dhyara::utils::http::counter(httpd_req_t* req){
    std::stringstream response_json;
    response_json << "{";
    response_json << "\"beacon\":" << "{" "\"tx\":" << _link.tx(dhyara::packets::type::beacon) << "," "\"rx\":" << _link.rx(dhyara::packets::type::beacon) << "}" ",";
    response_json << "\"acknowledgement\":" << "{" "\"tx\":" << _link.tx(dhyara::packets::type::acknowledgement) << "," "\"rx\":" << _link.rx(dhyara::packets::type::acknowledgement) << "}" ",";
    response_json << "\"advertisement\":" << "{" "\"tx\":" << _link.tx(dhyara::packets::type::advertisement) << "," "\"rx\":" << _link.rx(dhyara::packets::type::advertisement) << "}" ",";
    response_json << "\"chunk\":" << "{" "\"tx\":" << _link.tx(dhyara::packets::type::chunk) << "," "\"rx\":" << _link.rx(dhyara::packets::type::chunk) << "}" ",";
    response_json << "\"delivered\":" << "{" "\"tx\":" << _link.tx(dhyara::packets::type::delivered) << "," "\"rx\":" << _link.rx(dhyara::packets::type::delivered) << "}" ",";
    response_json << "\"echo_request\":" << "{" "\"tx\":" << _link.tx(dhyara::packets::type::echo_request) << "," "\"rx\":" << _link.rx(dhyara::packets::type::echo_request) << "}" ",";
    response_json << "\"echo_reply\":" << "{" "\"tx\":" << _link.tx(dhyara::packets::type::echo_reply) << "," "\"rx\":" << _link.rx(dhyara::packets::type::echo_reply) << "}" ",";
    response_json << "\"echo_lost\":" << "{" "\"tx\":" << _link.tx(dhyara::packets::type::echo_lost) << "," "\"rx\":" << _link.rx(dhyara::packets::type::echo_lost) << "}";
    response_json << "}";
    
    std::string response = response_json.str();
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response.c_str(), response.length());
    return ESP_OK;
}

esp_err_t dhyara::utils::http::routes_html(httpd_req_t* req){
    static const char* wrapper_close = 
                "</div>" 
                "<script>"
                "setTimeout(() => {"
                    "fetch_banner();"
                    "poll_routing();"
                "}, 500);"
                "</script>"
            "</body>";
        
    httpd_resp_set_type(req, "text/html");
    httpd_resp_sendstr_chunk(req, html_open);
    httpd_resp_sendstr_chunk(req, (const char*)dhyara::assets::head_html_start);
    httpd_resp_sendstr_chunk(req, wrapper_open);
    httpd_resp_sendstr_chunk(req, fragments::sidebar);
    httpd_resp_sendstr_chunk(req, central_open);
    httpd_resp_sendstr_chunk(req, fragments::banner);
    httpd_resp_sendstr_chunk(req, fragments::routing);
    httpd_resp_sendstr_chunk(req, central_close);
    httpd_resp_sendstr_chunk(req, wrapper_close);
    httpd_resp_sendstr_chunk(req, html_close);
    httpd_resp_sendstr_chunk(req, 0x0);
    return ESP_OK;
}

esp_err_t dhyara::utils::http::routes(httpd_req_t* req){
    std::stringstream response_json;
    response_json << "{";
    
    {
        std::stringstream routes_json;
        routes_json << "[";
        for(auto it = _link.routes().route_begin(); it != _link.routes().route_end(); ++it){
            if(it != _link.routes().route_begin()){
                routes_json << ",";
            }
            routes_json << "{";
            routes_json << "\"dst\":" << '"' << it->first.dst().to_string() << '"' << ",";
            routes_json << "\"via\":" << '"' << it->first.via().to_string() << '"' << ",";
            routes_json << "\"delay\":" << (double)it->second.delay()/1000.0 << ",";
            routes_json << "\"hops\":" << (int)it->second.hops() << ",";
            routes_json << "\"updated\":" << it->second.updated();
            routes_json << "}";
        }
        routes_json << "]";
        response_json << "\"routes\":" << routes_json.str();
    }
    response_json << ",";
    {
        std::stringstream next_json;
        next_json << "[";
        for(auto it = _link.routes().next_begin(); it != _link.routes().next_end(); ++it){
            if(it != _link.routes().next_begin()){
                next_json << ",";
            }
            next_json << "{";
            next_json << "\"dst\":" << '"' << it->first.to_string() << '"' << ",";
            next_json << "\"via\":" << '"' << it->second.via().to_string() << '"' << ",";
            next_json << "\"delay\":" << (double)it->second.delay()/1000.0 << ",";
            next_json << "\"hops\":" << (int)it->second.hops();
            next_json << "}";
        }
        next_json << "]";
        response_json << "\"next\":" << next_json.str();
    }
    response_json << "}";
    std::string response = response_json.str();
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response.c_str(), response.length());
    return ESP_OK;
}

esp_err_t dhyara::utils::http::peers_html(httpd_req_t* req){
    static const char* wrapper_close = 
                "</div>" 
                "<script>"
                "setTimeout(() => {"
                    "fetch_banner();"
                    "poll_peers();"
                "}, 500);"
                "</script>"
            "</body>";
        
    httpd_resp_set_type(req, "text/html");
    httpd_resp_sendstr_chunk(req, html_open);
    httpd_resp_sendstr_chunk(req, (const char*)dhyara::assets::head_html_start);
    httpd_resp_sendstr_chunk(req, wrapper_open);
    httpd_resp_sendstr_chunk(req, fragments::sidebar);
    httpd_resp_sendstr_chunk(req, central_open);
    httpd_resp_sendstr_chunk(req, fragments::banner);
    httpd_resp_sendstr_chunk(req, fragments::peers);
    httpd_resp_sendstr_chunk(req, central_close);
    httpd_resp_sendstr_chunk(req, wrapper_close);
    httpd_resp_sendstr_chunk(req, html_close);
    httpd_resp_sendstr_chunk(req, 0x0);
    return ESP_OK;
}


esp_err_t dhyara::utils::http::peers(httpd_req_t* req){
    std::stringstream response_json;
    response_json << "{";
    {
        std::stringstream neighbours_json;
        neighbours_json << "[";
        for(auto it = _link.neighbours().begin(); it != _link.neighbours().end(); ++it){
            if(it != _link.neighbours().begin()){
                neighbours_json << ",";
            }
            const dhyara::neighbour& neighbour = it->second;
            neighbours_json << "{";
            neighbours_json << "\"mac\":" << '"' << neighbour.addr().to_string() << '"' << ",";
            neighbours_json << "\"name\":" << '"' << neighbour.name() << '"' << ",";
            neighbours_json << "\"channel\":" << (int)neighbour.channel() << ",";
            neighbours_json << "\"rssi\":" << (int)neighbour.rssi() << ",";
            neighbours_json << "\"encrypt\":" << std::boolalpha << neighbour.encrypt();
            neighbours_json << "}";
        }
        neighbours_json << "]";
        response_json << "\"neighbours\":" << neighbours_json.str();
    }
    response_json << ",";
    {
        std::stringstream universe_json;
        universe_json << "[";
        for(auto it = _link.universe().begin(); it != _link.universe().end(); ++it){
            if(it != _link.universe().begin()){
                universe_json << ",";
            }
            const dhyara::peer& peer = it->second;
            universe_json << "{";
            universe_json << "\"mac\":" << '"' << peer.addr().to_string() << '"' << ",";
            universe_json << "\"name\":" << '"' << peer.name() << '"';
            universe_json << "}";
        }
        universe_json << "]";
        response_json << "\"universe\":" << universe_json.str();
    }
    
    response_json << "}";
    std::string response = response_json.str();
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response.c_str(), response.length());
    return ESP_OK;
}

#if CONFIG_ENABLE_SERVICES_OVER_HTTP

esp_err_t dhyara::utils::http::command(httpd_req_t* req){
    char content[128];
    size_t recv_size = std::min(req->content_len, sizeof(content));
    if(recv_size > sizeof(content)){
        httpd_resp_send_err(req, HTTPD_414_URI_TOO_LONG, "Command larger than 256 characters not allowed.");
        return ESP_OK;
    }else if(recv_size == 0){
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Empty command.");
        return ESP_OK;
    }

    std::fill(content, content +sizeof(content), 0);
    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }else{
            httpd_resp_send_500(req);
        }
        return ESP_OK;
    }

    std::vector<std::string> argv = detail::read_args(content);

    ESP_LOGI("dhyara-services", "Service `%s` requested", argv[0].c_str());
    if(!_registry.exists(argv[0])){ // if no such service is found
        httpd_resp_set_status(req, HTTPD_404);
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_sendstr_chunk(req, "No such service ");
        httpd_resp_sendstr_chunk(req, argv[0].c_str());
        httpd_resp_sendstr_chunk(req, "\n");
        httpd_resp_sendstr_chunk(req, "Following are the list of services registered");
        httpd_resp_sendstr_chunk(req, "\n");
        _registry.print(req);
        httpd_resp_sendstr_chunk(req, NULL);
        return ESP_OK;
    }

    _registry.run(req, argv.cbegin(), argv.cend());
    return ESP_OK;
}

#endif
/*
 * Copyright (c) 2020, Neel Basu <neel.basu.z@gmail.com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY Neel Basu <neel.basu.z@gmail.com> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Neel Basu <neel.basu.z@gmail.com> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "dhyara/utils/http.h"
#include <sstream>
#include "dhyara/assets.h"
#include <cstring>
#include "esp_wifi.h"
#include <dhyara/dhyara.h>

// namespace texts{
// 
// static const char* banner = 
//     "<div class='full-w'>"
//         "<div class='board banner'>"
//             "<div id='info-iface' class='groups'>"
//                 "<div class='group'>"
//                     "<div class='cell' data-key='ssid'></div>"
//                     "<div class='cell' data-key='mac'></div>"
//                 "</div>"
//                 "<div class='group'>"
//                     "<div class='cell' data-key='channel'></div>"
//                     "<div class='cell' data-key='frequency'></div>"
//                 "</div>"
//                 "<div class='group'>"
//                     "<div class='cell' data-key='power saving'></div>"
//                     "<div class='cell' data-key='max power'></div>"
//                 "</div>"
//                 "<div class='group'>"
//                     "<div class='cell' data-key='authentication'></div>"
//                     "<div class='cell' data-key='max clients'></div>"
//                 "</div>"
//             "</div>"
//         "</div>"
//     "</div>";
// 
// static const char* sidebar = 
//     "<div class='sidebar'>"
//         "<div class='logo'>Dhyara</div>"
//         "<ul id='menu' class='contents'>"
//             "<li data-label='Home' data-icon='home' data-href='/' />"
//             "<li data-label='Routing' data-icon='routing' data-href='/routing' />"
//             "<li data-label='Peers' data-icon='neighbourhood' data-href='#' />"
//         "</ul>"
//     "</div>";
//     
// static const char* home = 
//     "<div class='full-flex-hz'>"
//         "<div class='board cluster'>"
//             "<div class='heading'>Dhyara Configuration</div>"
//             "<div class='groups' id='info-dhyara'>"
//                 "<div class='group'>"
//                     "<div class='cell' data-key='send queueing'></div>"
//                     "<div class='cell' data-key='queue size'></div>"
//                     "<div class='cell' data-key='sync queue size'></div>"
//                     "<div class='cell' data-key='broadcast channel'></div>"
//                     "<div class='cell' data-key='peer channel'></div>"
//                 "</div>"
//                 "<div class='group'>"
//                     "<div class='cell' data-key='beacon interval'></div>"
//                     "<div class='cell' data-key='delay tolerance'></div>"
//                     "<div class='cell' data-key='depreciation coefficient'></div>"
//                     "<div class='cell' data-key='advertisement expiry'></div>"
//                     "<div class='cell' data-key='route expiry'></div>"
//                 "</div>"
//             "</div>"
//         "</div>"
//         "<div class='board cluster'>"
//             "<div class='heading'>Counters</div>"
//             "<div class='groups' id='counter'>"
//                 "<div class='group'>"
//                     "<div class='cell' data-key='beacon'><span class='value' /></div>"
//                     "<div class='cell' data-key='acknowledgement'><span class='value' /></div>"
//                     "<div class='cell' data-key='advertisement'><span class='value' /></div>"
//                     "<div class='cell' data-key='chunk'><span class='value' /></div>"
//                     "<div class='cell' data-key='delivered'><span class='value' /></div>"
//                 "</div>"
//                 "<div class='group'>"
//                     "<div class='cell' data-key='icmp request'><span class='value' /></div>"
//                     "<div class='cell' data-key='icmp reply'><span class='value' /></div>"
//                     "<div class='cell' data-key='icmp lost'><span class='value' /></div>"
//                 "</div>"
//             "</div>"
//         "</div>"
//     "</div>";
//     
// static const char* routing = 
//     "<div class='full-flex-hz'>"
//         "<div class='board cluster-full'>"
//             "<div class='heading'>Routing Table</div>"
//             "<table>"
//                 "<thead>"
//                     "<tr>"
//                         "<th>Destination</th>"
//                         "<th>Intermediate</th>"
//                         "<th>Delay</th>"
//                         "<th>Updated</th>"
//                     "</tr>"
//                 "</thead>"
//                 "<tbody id='routes-body'></tbody>"
//             "</table>"
//         "</div>"
//     "</div>"
//     "<div class='full-flex-hz'>"
//         "<div class='board cluster-full'>"
//             "<div class='heading'>Best Route</div>"
//             "<table>"
//                 "<thead>"
//                     "<tr>"
//                         "<th>Destination</th>"
//                         "<th>Intermediate</th>"
//                         "<th>Delay</th>"
//                     "</tr>"
//                 "</thead>"
//                 "<tbody id='next-body'></tbody>"
//             "</table>"
//         "</div>"
//     "</div>";
//     
// }

dhyara::utils::http::http(dhyara::link& link): _link(link), _config(HTTPD_DEFAULT_CONFIG()), _server(0x0),
    _index   (httpd_uri_t{"/",             HTTP_GET, dhyara::utils::http::index_handler,   this}),
    _routing (httpd_uri_t{"/routing",      HTTP_GET, dhyara::utils::http::routing_handler, this}),
    _style   (httpd_uri_t{"/dhyara.css",   HTTP_GET, dhyara::utils::http::style_handler,   this}),
    _icons   (httpd_uri_t{"/icons",        HTTP_GET, dhyara::utils::http::icons_handler,   this}),
    _info    (httpd_uri_t{"/info.json",    HTTP_GET, dhyara::utils::http::info_handler,    this}),
    _counter (httpd_uri_t{"/counter.json", HTTP_GET, dhyara::utils::http::counter_handler, this}),
    _routes  (httpd_uri_t{"/routes.json",  HTTP_GET, dhyara::utils::http::routes_handler,  this})
{}


esp_err_t dhyara::utils::http::start(){
    esp_err_t res = httpd_start(&_server, &_config);
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_index);
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_routing);
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_style);
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_icons);
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_info);
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_counter);
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_routes);
    return res;
}

esp_err_t dhyara::utils::http::index_handler(httpd_req_t* req){
    dhyara::utils::http* self = static_cast<dhyara::utils::http*>(req->user_ctx);
    return self->index(req);
}

esp_err_t dhyara::utils::http::style_handler(httpd_req_t* req){
    dhyara::utils::http* self = static_cast<dhyara::utils::http*>(req->user_ctx);
    return self->style(req);
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

esp_err_t dhyara::utils::http::routing_handler(httpd_req_t* req){
    dhyara::utils::http* self = static_cast<dhyara::utils::http*>(req->user_ctx);
    return self->routing(req);
}

esp_err_t dhyara::utils::http::routes_handler(httpd_req_t* req){
    dhyara::utils::http* self = static_cast<dhyara::utils::http*>(req->user_ctx);
    return self->routes(req);
}

esp_err_t dhyara::utils::http::index(httpd_req_t* req){
    const auto length = std::distance(dhyara::assets::index_html_start, dhyara::assets::index_html_end);
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char*)dhyara::assets::index_html_start, length);
    return ESP_OK;
}

esp_err_t dhyara::utils::http::icons(httpd_req_t* req){
    const auto len = strlen(req->uri);
    static const std::string neighbourhood = "neighbourhood";
    static const std::string home = "home";
    static const std::string routing = "routing";
    static const std::string universe = "universe";
    
    if(len <= 7){
        httpd_resp_send_404(req);
    }else{
        std::string query(req->uri+7);
        if(query.compare(0, neighbourhood.size(), neighbourhood) == 0){
            httpd_resp_set_type(req, "image/gif");
            httpd_resp_send(req, (const char*)dhyara::assets::neighbourhood_gif_start, std::distance(dhyara::assets::neighbourhood_gif_start, dhyara::assets::neighbourhood_gif_end));
        }else if(query.compare(0, home.size(), home) == 0){
            httpd_resp_set_type(req, "image/gif");
            httpd_resp_send(req, (const char*)dhyara::assets::home_gif_start, std::distance(dhyara::assets::home_gif_start, dhyara::assets::home_gif_end));
        }else if(query.compare(0, routing.size(), routing) == 0){
            httpd_resp_set_type(req, "image/gif");
            httpd_resp_send(req, (const char*)dhyara::assets::routing_gif_start, std::distance(dhyara::assets::routing_gif_start, dhyara::assets::routing_gif_end));
        }else if(query.compare(0, universe.size(), universe) == 0){
            httpd_resp_set_type(req, "image/gif");
            httpd_resp_send(req, (const char*)dhyara::assets::universe_gif_start, std::distance(dhyara::assets::universe_gif_start, dhyara::assets::universe_gif_end));
        }else{
            httpd_resp_send_404(req);
        }
    }
    return ESP_OK;
}

esp_err_t dhyara::utils::http::style(httpd_req_t* req){
    const auto length = std::distance(dhyara::assets::dhyara_css_start, dhyara::assets::dhyara_css_end);
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
            case WIFI_AUTH_WAPI_PSK:
                auth_str = "WIFI_AUTH_WAPI_PSK";
                break;
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

esp_err_t dhyara::utils::http::routing(httpd_req_t* req){
    const auto length = std::distance(dhyara::assets::routing_html_start, dhyara::assets::routing_html_end);
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char*)dhyara::assets::routing_html_start, length);
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
            next_json << "\"delay\":" << (double)it->second.delay()/1000.0;
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

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

dhyara::utils::http::http(dhyara::link& link): _link(link), _config(HTTPD_DEFAULT_CONFIG()), _server(0x0),
    _routes(httpd_uri_t{"/routes", HTTP_GET, dhyara::utils::http::routes_handler, this}),
    _info(httpd_uri_t{"/info.json", HTTP_GET, dhyara::utils::http::info_handler, this}) 
{}


esp_err_t dhyara::utils::http::start(){
    esp_err_t res = httpd_start(&_server, &_config);
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_routes);
    if (res != ESP_OK) return res; else res = httpd_register_uri_handler(_server, &_info);
    return res;
}


esp_err_t dhyara::utils::http::routes_handler(httpd_req_t* req){
    dhyara::utils::http* self = static_cast<dhyara::utils::http*>(req->user_ctx);
    return self->routes(req);
}

esp_err_t  dhyara::utils::http::info_handler(httpd_req_t* req){
    dhyara::utils::http* self = static_cast<dhyara::utils::http*>(req->user_ctx);
    return self->info(req);
}

esp_err_t dhyara::utils::http::routes(httpd_req_t* req){
    std::stringstream table;
    table << "<table class='paleBlueRows'>"
            << "<tr>"
                << "<th>" << "destination" << "</th>"
                << "<th>" << "intermediate" << "</th>"
                << "<th>" << "delay (ms)" << "</th>"
                << "<th>" << "updated" << "</th>"
            << "</tr>";

    for(auto it = _link.routes().route_begin(); it != _link.routes().route_end(); ++it){
        const auto& dst         = it->first.dst();
        const auto& via         = it->first.via();
        const auto& delay       = it->second.delay();
        const auto& updated     = it->second.updated();
        
        table << "<tr>"
                << "<td>" << dst.to_string() << "</td>"
                << "<td>" << via.to_string() << "</td>"
                << "<td>" << (double)delay/1000.0 << "</td>"
                << "<td>" << updated << "</td>"
            << "</tr>";
    }
    table << "</table>";
    
    table << "<table class='paleBlueRows'>"
            << "<tr>"
                << "<th>" << "destination" << "</th>"
                << "<th>" << "next" << "</th>"
                << "<th>" << "delay (ms)" << "</th>"
                << "<th>" << "RSSI" << "</th>"
                << "<th>" << "Name" << "</th>"
            << "</tr>";
    for(auto it = _link.routes().next_begin(); it != _link.routes().next_end(); ++it){
        const auto& dst         = it->first;
        const auto& via         = it->second.via();
        const auto& delay       = it->second.delay();
        std::int8_t rssi = 0;
        std::string name;
        if(_link.neighbours().exists(dst)){
            const auto& peer = _link.neighbours().get_peer(dst);
            rssi = peer.rssi();
            name = peer.name();
        }
        
        table << "<tr>"
                << "<td>" << dst.to_string() << "</td>"
                << "<td>" << via.to_string() << "</td>"
                << "<td>" << (double)delay/1000.0 << "</td>"
                << "<td>" << (int)rssi << " dBm" << "</td>"
                << "<td>" << name << "</td>"
            << "</tr>";
    }
    table << "</table>";
    
    std::string style(dhyara::assets::routing_css_start, dhyara::assets::routing_css_end);
    
    std::stringstream html;
    html << "<html>"
            << "<head>" 
                << "<title>" << "DHYARA ROUTING TABLE" << "</title>" 
                << "<style>" << style << "</style>"
            << "</head>"
            << "<body>" << table.str() << "</body>"
        << "</html>";
    
    std::string html_str = html.str();
    
    httpd_resp_send(req, html_str.c_str(), html_str.length());
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
        
    }
    
    response_json << "}";
    
    std::string response = response_json.str();
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response.c_str(), response.length());
    return ESP_OK;
}

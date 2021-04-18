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

dhyara::utils::http::http(dhyara::link& link): _link(link), _config(HTTPD_DEFAULT_CONFIG()), _server(0x0),
    _routes(httpd_uri_t{"/routes", HTTP_GET, dhyara::utils::http::routes_handler, this}) 
{

}


esp_err_t dhyara::utils::http::start(){
    esp_err_t res = httpd_start(&_server, &_config);
    if (res == ESP_OK) {
        res = httpd_register_uri_handler(_server, &_routes);
    }
    return res;
}


esp_err_t dhyara::utils::http::routes_handler(httpd_req_t* req){
    dhyara::utils::http* self = static_cast<dhyara::utils::http*>(req->user_ctx);
    return self->routes(req);
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

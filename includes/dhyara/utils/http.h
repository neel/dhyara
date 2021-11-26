/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_HTTP_H
#define DHYARA_HTTP_H

#include <esp_http_server.h>
#include "dhyara/link.h"

#if CONFIG_ENABLE_SERVICES_OVER_HTTP

#include "dhyara/services/registry.hpp"
#include "dhyara/services/service.hpp"

namespace dhyara{
    namespace services{
        struct identify;
        #if CONFIG_ENABLE_SERVICE_ROUTES
            struct routes;
        #endif 
        #if CONFIG_ENABLE_SERVICE_UNIVERSE
            struct universe;
        #endif 
        #if CONFIG_ENABLE_SERVICE_PING
            struct ping;
        #endif 
        #if CONFIG_ENABLE_SERVICE_PHY_RATE
            struct phy_rate;
        #endif 
    }
}

#endif

namespace dhyara{
namespace utils{
    
/**
 * HTTP utility to monitor the routing table
 * \ingroup utils
 */
struct http{
    
    /**
     * The HTTP Management console requires a reference to the link in order to access the routes and the neighbourhood.
     * \param link communication link
     */
    http(dhyara::link& link);
    /**
     * Start the HTTP Server
     */
    esp_err_t start();
    
    private:
        dhyara::link&  _link;
        httpd_config_t _config;
        httpd_handle_t _server;
    private:
        httpd_uri_t    _index_html;
        httpd_uri_t    _routes_html;
        httpd_uri_t    _peers_html;
        httpd_uri_t    _style;
        httpd_uri_t    _icons;
        httpd_uri_t    _info;
        httpd_uri_t    _counter;
        httpd_uri_t    _routes;
        httpd_uri_t    _peers;
#if CONFIG_ENABLE_SERVICES_OVER_HTTP
        httpd_uri_t    _command;
#endif
    private:
        static esp_err_t index_html_handler(httpd_req_t* req);
        static esp_err_t routes_html_handler(httpd_req_t* req);
        static esp_err_t peers_html_handler(httpd_req_t* req);
        static esp_err_t style_handler(httpd_req_t* req);
        static esp_err_t icons_handler(httpd_req_t* req);
        static esp_err_t info_handler(httpd_req_t* req);
        static esp_err_t counter_handler(httpd_req_t* req);
        static esp_err_t routes_handler(httpd_req_t* req);
        static esp_err_t peers_handler(httpd_req_t* req);
#if CONFIG_ENABLE_SERVICES_OVER_HTTP
        static esp_err_t command_handler(httpd_req_t* req);
#endif
    private:
        esp_err_t index_html(httpd_req_t* req);
        esp_err_t routes_html(httpd_req_t* req);
        esp_err_t peers_html(httpd_req_t* req);
        esp_err_t style(httpd_req_t* req);
        esp_err_t icons(httpd_req_t* req);
        esp_err_t info(httpd_req_t* req);
        esp_err_t counter(httpd_req_t* req);
        esp_err_t routes(httpd_req_t* req);
        esp_err_t peers(httpd_req_t* req);
#if CONFIG_ENABLE_SERVICES_OVER_HTTP
        esp_err_t command(httpd_req_t* req);
#endif 

#if CONFIG_ENABLE_SERVICES_OVER_HTTP
    private:
        using registry_type =  services::registry<
            services::identify
            #if CONFIG_ENABLE_SERVICE_ROUTES
                ,services::routes
            #endif 
            #if CONFIG_ENABLE_SERVICE_UNIVERSE
                ,services::universe
            #endif 
            #if CONFIG_ENABLE_SERVICE_PING
                ,services::ping
            #endif 
            #if CONFIG_ENABLE_SERVICE_PHY_RATE
                ,services::phy_rate
            #endif 
        >;

        registry_type       _registry;
#endif 

};

}

}

#endif // DHYARA_HTTP_H

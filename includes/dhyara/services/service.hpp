/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_SERVICES_SERVICE_H
#define DHYARA_SERVICES_SERVICE_H

#include <dhyara/cmd/cmd.hpp>
#include <esp_http_server.h>
#include "esp_err.h"
#include "esp_log.h"
#include <dhyara/services/stream.h>

namespace dhyara{
namespace services{

template <typename ServiceT>
struct service: private ServiceT{
    explicit service(httpd_req_t* req): _stream(req), _help(false){}

    /**
     * @brief Run the service from the HTTPD stack
     * 
     * @tparam InputIterator 
     * @param first 
     * @param last 
     * @return esp_err_t 
     */
    template<class InputIterator>
    esp_err_t operator()(InputIterator first, InputIterator last){
        bool responded = false;
        esp_err_t err = exec(first, last, responded);
        if(!responded){
            _run();
        }
        return err;
    }
    /**
     * @brief Run the service detached as a seperate FreeRTOS task
     * 
     * @tparam InputIterator 
     * @param req 
     * @param first 
     * @param last 
     * @return esp_err_t 
     */
    template<class InputIterator>
    static esp_err_t spawn(httpd_req_t* req, InputIterator first, InputIterator last){
        service<ServiceT>* svc = new service<ServiceT>(req);
        bool responded = false;
        esp_err_t err = svc->exec(first, last, responded);
        if(!responded){
            xTaskCreate(&service<ServiceT>::runner, ServiceT::name, ServiceT::stack_size, svc, ServiceT::priority, NULL);
            return ESP_OK;
        }else{
            delete svc;
            svc = 0x0;
            return err;
        }
    }
    static void runner(void* arg){
        service<ServiceT>* svc = reinterpret_cast<service<ServiceT>*>(arg);
        svc->_run();
        delete svc;
        svc = 0x0;
        vTaskDelete(NULL);
    }
    private:
        esp_err_t _run(){
            ESP_LOGI("dhyara-services", "Running service `%s`", ServiceT::name);
            return ServiceT::run(_stream);
        }
        template<class InputIterator>
        esp_err_t exec(InputIterator first, InputIterator last, bool& responded){
            using namespace dhyara;

            responded = true;
            auto options = (ServiceT::options(), cmd::arg("h", "help") & cmd::value(_help) % "show this help message");
            cmd::parse(options, first, last);
            if(!options.valid()){
                ESP_LOGI("dhyara-services", "Error parsing arguments for service `%s`", ServiceT::name);
                cmd::man(options, ServiceT::name, _stream);
                _stream.finish();
                return ESP_OK;
            }else if(_help){
                cmd::man(options, ServiceT::name, _stream);
                _stream.finish();
                return ESP_OK;
            }else{
                responded = false;
                return ESP_OK;
            }
        }
    private:
        services::stream _stream;
        bool             _help;
};

}
}


#endif // DHYARA_SERVICES_SERVICE_H
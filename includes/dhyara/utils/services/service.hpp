/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_SERVICES_SERVICE_H
#define DHYARA_SERVICES_SERVICE_H

#include <clipp/clipp.h>
#include <esp_http_server.h>
#include "esp_err.h"
#include "esp_log.h"
#include <dhyara/utils/services/stream.h>

namespace dhyara{
namespace utils{
namespace services{

template <typename ServiceT>
struct service: private ServiceT{
    explicit service(httpd_req_t* req): _req(req), _stream(req), _help(false){}

    template<class InputIterator>
    esp_err_t operator()(InputIterator first, InputIterator last){
        bool responded = false;
        esp_err_t err = exec(first, last, responded);
        if(!responded){
            _run();
        }
        return err;
    }
    template<class InputIterator>
    static esp_err_t spawn(httpd_req_t* req, InputIterator first, InputIterator last){
        service<ServiceT>* svc = new service<ServiceT>(req);
        bool responded = false;
        esp_err_t err = svc->exec(first, last, responded);
        if(!responded){
            httpd_queue_work(req->handle, runner, svc);
            return ESP_OK;
        }else{
            delete svc;
            svc = 0x0;
            return err;
        }
    }
    static void work_fn(void* arg){
        xTaskCreate(&service<ServiceT>::runner,    "run",    3*4096,  arg,   19,  NULL);
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
            ESP_LOGI("dhyara-services", "Running service `%s`", ServiceT::name());
            return ServiceT::run(_stream);
        }
        template<class InputIterator>
        esp_err_t exec(InputIterator first, InputIterator last, bool& responded){
            responded = true;
            auto options = ServiceT::options() | clipp::option("-h", "--help").set(_help) % "show this help message";
            if(!clipp::parse(first, last, options)){
                ESP_LOGI("dhyara-services", "Error parsing arguments for service `%s`", ServiceT::name());
                _stream << clipp::make_man_page(options, ServiceT::name());
                return _stream.finish(HTTPD_400);
            }else if(_help){
                _stream << clipp::make_man_page(options, ServiceT::name());
                return _stream.finish(HTTPD_200);
            }else{
                responded = false;
                return ESP_OK;
            }
        }
    private:
        httpd_req_t* _req;
        services::stream _stream;
        bool             _help;
};

}
}
}

#endif // DHYARA_SERVICES_SERVICE_H
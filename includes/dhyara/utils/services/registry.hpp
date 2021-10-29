/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_SERVICES_REGISTRY_H
#define DHYARA_SERVICES_REGISTRY_H

#include <string>
#include <type_traits>
#include <dhyara/utils/services/service.hpp>
#include <dhyara/utils/services/stream.h>
#include "esp_err.h"
#include "esp_log.h"

namespace dhyara{
namespace utils{
namespace services{

namespace detail{
    template <typename Head, typename... Tail>
    struct basic_registry: private basic_registry<Tail...>{
        using tail = basic_registry<Tail...>;
        
        template <typename X>
        static constexpr bool has() { 
            return std::is_same<X, Head>::value || tail::template has<X>(); 
        }
        
        bool exists(const std::string& name) const {
            return (name == Head::name()) || tail::exists(name);
        }
        
        template <typename InputIterator>
        esp_err_t run(httpd_req_t* req, InputIterator begin, InputIterator end) const{
            return run(req, *begin, begin+1, end);
        }

        private:
            template <typename InputIterator>
            esp_err_t run(httpd_req_t* req, const std::string& name, InputIterator begin, InputIterator end) const{
                if(name == Head::name()){
                    ESP_LOGI("dhyara-services", "service `%s` found", name.c_str());
                    service<Head> service(req);
                    return service(begin, end);
                }else{
                    return tail::run(name, begin, end);
                }
            }

    };
    
    template <typename Head>
    struct basic_registry<Head>{
        template <typename X>
        static constexpr bool has() { 
            return std::is_same<X, Head>::value; 
        }
        
        bool exists(const std::string& name) const {
            return name == Head::name();
        }

        template <typename InputIterator>
        esp_err_t run(httpd_req_t* req, InputIterator begin, InputIterator end) const{
            return run(req, *begin, begin+1, end);
        }

        private:
            template <typename InputIterator>
            esp_err_t run(httpd_req_t* req, const std::string& name, InputIterator begin, InputIterator end) const{
                if(name == Head::name()){
                    ESP_LOGI("dhyara-services", "service `%s` found", name.c_str());
                    service<Head> service(req);
                    return service(begin, end);
                }
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "No such service");
                return ESP_OK;
            }
    };
}

template <typename... X>
using registry = detail::basic_registry<X...>;

}
}
}

#endif // DHYARA_SERVICES_REGISTRY_H
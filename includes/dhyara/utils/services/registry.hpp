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
        
        template <typename InputIterator, typename StreamT>
        const char* run(InputIterator begin, InputIterator end, StreamT& stream) const{
            return run(*begin, begin+1, end, stream);
        }

        private:
            template <typename InputIterator, typename StreamT>
            const char* run(const std::string& name, InputIterator begin, InputIterator end, StreamT& stream) const{
                if(name == Head::name()){
                    service<Head, StreamT> service(stream);
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

        template <typename InputIterator, typename StreamT>
        const char* run(InputIterator begin, InputIterator end, StreamT& stream) const{
            return run(*begin, begin+1, end, stream);
        }

        private:
            template <typename InputIterator, typename StreamT>
            const char* run(const std::string& name, InputIterator begin, InputIterator end, StreamT& stream) const{
                if(name == Head::name()){
                    service<Head, StreamT> service(stream);
                    return service(begin, end);
                }
                return HTTPD_404;
            }
    };
}

template <typename... X>
using registry = detail::basic_registry<X...>;

}
}
}

#endif // DHYARA_SERVICES_REGISTRY_H
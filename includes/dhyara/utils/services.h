/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_SERVICES_H
#define DHYARA_SERVICES_H

#include <cstdint>
#include <string>
#include <dhyara/address.h>
#include <clipp/clipp.h>
#include <sstream>

namespace dhyara{
    
namespace services{
    
template <bool OutputBuffering = true>
struct stream{
    std::stringstream _stream;
};
    
template <typename ServiceT>
struct service;
    
namespace detail{
    template <typename Head, typename... Tail>
    struct basic_registry: private basic_registry<Tail...>{
        using tail = basic_registry<Tail...>;
        
        template <typename X>
        static constexpr bool exists() { 
            return std::is_same<X, Head>::value || tail::template exists<X>(); 
        }
        
        bool matches(const std::string& name) const {
            return ((name == Head::name()) || tail::matches(name));
        }
        
        template <typename InputIterator, bool OutputBuffering>
        stream<OutputBuffering>& run(const std::string& name, InputIterator begin, InputIterator end, stream<OutputBuffering>& stream) const{
            if(name == Head::name()){
                service<Head> service(begin, end);
                service(stream);
            }else{
                tail::run(name, begin, end);
            }
            return stream;
        }
    };
    
    template <typename Head>
    struct basic_registry<Head>{
        std::stringstream _stream;
        
        template <typename X>
        static constexpr bool exists() { 
            return std::is_same<X, Head>::value; 
        }
        
        bool matches(const std::string& name) const {
            return (name == Head::name());
        }
        
        template <typename InputIterator, bool OutputBuffering>
        stream<OutputBuffering>& run(const std::string& name, InputIterator begin, InputIterator end, stream<OutputBuffering>& stream) const{
            if(name == Head::name()){
                service<Head> service(begin, end);
                service(stream);
            }
            return stream;
        }
    };
}

template <typename... X>
using registry = detail::basic_registry<X...>;

template <typename ServiceT>
struct service{
    template<class InputIterator>
    void parse(InputIterator first, InputIterator last){
        auto options = self().options();
        self().clear();
        if(!clipp::parse(first, last, options)){
            clipp::man_page man = clipp::make_man_page(options, ServiceT::name);
            // respond with man page
            return;
        }
    }
    
    private:
        ServiceT& self() { return static_cast<ServiceT&>(*this); }
};

struct ping;

struct ping_service{
    friend struct services::ping;

    private:
        explicit ping_service(const dhyara::address& target, std::uint32_t batch);
    private:
        dhyara::address _target;
        std::uint32_t   _batch;
        std::uint32_t   _count;
        std::uint32_t   _sleep;
        std::uint8_t    _wait;
};

struct ping: service<ping>{
    using service = ping_service;

    constexpr static const char* name = "ping";
    
    inline ping() {
        clear();
    }
    
    inline void clear() {
        _target = "";
        _batch = 20;
        _wait = 2;
    }
    inline clipp::group options() {
        return (
            clipp::value("target address", _target),
            clipp::option("-b", "--batch").set(_batch).doc("number of requests in one batch"),
            clipp::option("-w", "--wait").set(_wait).doc("number of seconds to wait for receiving all responses")
        );
    }
    inline void run(){
        
    }
    private:
        std::string  _target;
        std::uint8_t _batch;
        std::uint8_t _wait;
};

}
}

#endif // DHYARA_SERVICES_H

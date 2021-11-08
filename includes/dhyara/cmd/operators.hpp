/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_CMD_OPERATORS_H
#define DHYARA_CMD_OPERATORS_H

#include "fwd.hpp"
#include "detail.hpp"

namespace dhyara{
namespace cmd{

template <typename V>
param<V>& operator%(param<V>& p, const std::string& desc){
    p.description(desc);
    return p;
}
template <typename V>
param<V> operator%(param<V>&& p, const std::string& desc){
    param<V> pn(std::move(p));
    pn.description(desc);
    return pn;
}

namespace detail{
    template <typename V>
    param<V> operator&(const detail::param_name& pn, const detail::param_value<V>& pv){
        param<V> p(pn._option, pn._name, pv._value, pn.prefix());
        p.placeholder(pv._placeholder);
        p.required(pv._required);
        if(!pv._description.empty()){
            p.description(pv._description);
        }
        return p;
    }
    template <typename V>
    detail::param_value<V>& operator%(detail::param_value<V>& p, const std::string& desc){
        p._description = desc;
        return p;
    }
    template <typename V>
    detail::param_value<V> operator%(detail::param_value<V>&& p, const std::string& desc){
        detail::param_value<V> pv(std::move(p));
        pv._description = desc;
        return pv;
    }

}

template <typename X, typename Y>
chain<param<X>, chain<param<Y>>> operator,(const param<X>& x, const param<Y>& y){
    return chain<param<X>, chain<param<Y>>>(x, chain<param<Y>>(y));
}

template <typename X>
chain<param<X>, void> operator,(const chain<void, void>&, const param<X>& x){
    return chain<param<X>, void>(x);
}

template <typename T, typename Head, typename Tail, typename std::enable_if<!std::is_void<Tail>::value, bool>::type = true >
typename chain<Head, Tail>::template extend<T> operator,(const chain<Head, Tail>& c, const param<T>& p){
    return c.upgrade(p);
}

}
}

#endif // DHYARA_CMD_OPERATORS_H
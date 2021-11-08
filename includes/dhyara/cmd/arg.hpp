/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_CMD_ARG_H
#define DHYARA_CMD_ARG_H

#include "fwd.hpp"
#include "detail.hpp"
#include "param.hpp"

namespace dhyara{
namespace cmd{

inline detail::param_name arg(const std::string& option, const std::string& name = ""){
    return detail::param_name(option, name);
}

template <typename V>
detail::param_value<V> value(V& value, const std::string& placeholder = "", bool required = false){
    return detail::param_value<V>(placeholder, value, required);
}

template <typename V>
param<V> pos(const std::string& placeholder, V& value, bool required = false){
    param<V> p(value);
    p.placeholder(placeholder);
    p.prefix("");
    p.required(required);
    return p;
}

}
}

#endif // DHYARA_CMD_ARG_H
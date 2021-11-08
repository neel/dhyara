/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_CMD_DETAIL_H
#define DHYARA_CMD_DETAIL_H

#include "fwd.hpp"
#include <string>
#include <iterator>

namespace dhyara{
namespace cmd{
namespace detail{

template<class ForwardIt1, class ForwardIt2>
constexpr ForwardIt1 search(ForwardIt1 first, ForwardIt1 last, ForwardIt2 s_first, ForwardIt2 s_last){
    while (true) {
        ForwardIt1 it = first;
        for (ForwardIt2 s_it = s_first; ; ++it, ++s_it) {
            if (s_it == s_last) return first;
            if (it == last)  return last;
            if (!(*it == *s_it)) break;
        }
        ++first;
    }
}

template <typename T, typename ArgT>
static auto match_param(const param<T>& p, const ArgT& arg) -> decltype(arg.begin()){
    auto begin = std::begin(arg);
    auto end   = std::end(arg);
    std::string name = p.name(), option = p.option();
    
    // search for the name that starts with double hyphens
    auto it = name.empty() ? end : detail::search(begin, end, name.begin(), name.end());
    if(it != end){
        std::advance(it, name.size()-1);
    }else{
        // search for the option that starts with single hyphen
        it = option.empty() ? end : detail::search(begin, end, option.begin(), option.end());
        if(it != end){
            std::advance(it, option.size()-1);
        }
    }
    return it;
}

template <typename V>
struct param_value{
    V& _value;
    bool _required;
    std::string _placeholder;
    std::string _description;

    param_value(V& value, bool required=false): _value(value), _required(required) {}
    param_value(const std::string& placeholder, V& value, bool required=false): _value(value), _required(required), _placeholder(placeholder) {}
    param_value(const param_value<V>&) = default;
    
    bool is_required() const { return _required; }
    bool is_optional() const { return !_required; }
};
struct param_name{
    std::string _prefix;
    std::string _option;
    std::string _name;

    inline explicit param_name(const std::string& option): _prefix("-"), _option(option) {}
    inline param_name(const std::string& option, const std::string& name, const std::string& prefix="-"):  _prefix(prefix), _option(option), _name(name) {}
    inline param_name(const param_name&) = default;
    
    inline const std::string& prefix() const { return _prefix; }
    inline param_name& prefix(const std::string& prefix) { _prefix = prefix; return *this; }
};

}
}
}

#endif // DHYARA_CMD_DETAIL_H
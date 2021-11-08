/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_CMD_PARAM_H
#define DHYARA_CMD_PARAM_H

#include <string>
#include <sstream>
#include <iterator>
#include "fwd.hpp"

namespace dhyara{
namespace cmd{

template <typename T>
struct param{
    explicit param(T& value): _value(value), _is_set(false), _required(false), _valid(false), _prefix("-"), _placeholder("V") {}
    param(const std::string& option, T& value, const std::string& prefix="-"): _value(value), _is_set(false), _required(false), _valid(false), _prefix(prefix) , _option(option), _placeholder("V") {}
    param(const std::string& option, const std::string& name, T& value, const std::string& prefix="-"): _value(value), _is_set(false), _required(false), _valid(false), _prefix(prefix), _option(option), _name(name), _placeholder("V") {}
    param(const param<T>&) = default;

    std::string option() const { return _prefix + _option; }
    std::string name() const { return _prefix + (_prefix + _name); }
    const T& value() const { return _value; }
    const std::string& placeholder() const { return _placeholder; }
    const std::string& description() const { return _description; }
    const std::string& prefix() const { return _prefix; }

    param<T>& option(const std::string& option) { _option = option; return *this; }
    param<T>& name(const std::string& name) { _name = name; return *this; }
    param<T>& placeholder(const std::string& placeholder) { _placeholder = placeholder; return *this; }
    param<T>& description(const std::string& description) { _description = description; return *this; }
    param<T>& prefix(const std::string& prefix) { _prefix = prefix; return *this; }
    param<T>& required(bool req) { _required = req; return *this; }

    bool required() const { return _required; }
    bool optional() const { return !_required; }
    bool valid() const    { return _valid; }
    
    bool is_set() const { return _is_set; }
    void read(std::stringstream& stream){
        stream >> _value;
        _valid = ((stream.rdstate() & std::ios::failbit) == 0);
        _is_set = true;
    }
    /**
     * @brief Assuming that the key has been found, now parse the value
     * 
     * @tparam StrIt 
     * @tparam VectorIt 
     * @param v_it position of the vector on which the key of the param is found
     * @param v_end end position of the argument vector
     * @param a_begin begining position of the value of this param
     * @param a_end end position of the argument in which teh key has been found
     * @return VectorIt 
     */
    template <typename StrIt, typename VectorIt>
    VectorIt parse(VectorIt v_it, VectorIt v_end, StrIt a_it, StrIt a_end){
        auto rest = std::distance(a_it, a_end);
        std::stringstream stream;
        if(rest > 0){
            // value provided in head
            std::copy(a_it, a_end, std::ostream_iterator<char>(stream));
            read(stream);
            return v_it+1;
        }else{
            // value provided in tail
            if(v_it+1 != v_end) {
                stream << *(++v_it);
                read(stream);
                return v_it+1;
            }else{
                // TODO error: end reached while reading value for _head.name()
                return v_end;
            }
        }
    }
    template <typename StreamT>
    StreamT& usage(StreamT& stream) const {
        if(optional()) stream << "[ ";
        if(!_prefix.empty()){
            if(!_option.empty()){
                stream << _prefix << _option << "<" << _placeholder << ">";
            }else if(!_name.empty()){
                stream << _prefix << _prefix << _name << " " << "<" << _placeholder << ">";
            }
        }else{
            stream << "<" << _placeholder << ">";
        }
        if(optional()) stream << " ]";
        return stream;
    }
    template <typename StreamT>
    StreamT& desc(StreamT& stream) const {
        std::size_t chars_written = 0;
        if(!_prefix.empty()){
            if(!_option.empty()){
                stream << _prefix << _option << "<" << _placeholder << ">";
                chars_written += _prefix.size() + _option.size() + _placeholder.size() + 2;
            }
            if(!_option.empty() && !_name.empty()){
                stream << ", ";
                chars_written += 2;
            }
            if(!_name.empty()){
                stream << _prefix << _prefix << _name << " " << "<" << _placeholder << ">";
                chars_written += 2*_prefix.size() + _name.size() + 1 + _placeholder.size() + 2;
            }
        }else{
            stream << "<" << _placeholder << ">";
            chars_written += _placeholder.size() + 2;
        }
        if(chars_written < 25){
            std::string space(25-chars_written, ' ');
            stream << space;
        }
        stream << " " << _description;
        return stream;
    }
    private:
        T& _value;
        bool _is_set;
        bool _required;
        bool _valid;
        std::string _prefix;
        std::string _option;
        std::string _name;
        std::string _placeholder;
        std::string _description;
};

template <>
struct param<bool>{
    explicit param(bool& value): _value(value), _is_set(false), _required(false), _valid(false), _prefix("-") {}
    param(const std::string& option, bool& value, const std::string& prefix="-"): _value(value), _is_set(false), _required(false), _valid(false), _prefix(prefix) , _option(option){}
    param(const std::string& option, const std::string& name, bool& value, const std::string& prefix="-"): _value(value), _is_set(false), _required(false), _valid(false), _prefix(prefix), _option(option), _name(name){}
    param(const param<bool>&) = default;

    std::string option() const { return _prefix + _option; }
    std::string name() const { return _prefix + (_prefix + _name); }
    bool value() const { return _value; }
    // const std::string& placeholder() const { return _placeholder; }
    const std::string& description() const { return _description; }
    const std::string& prefix() const { return _prefix; }

    param<bool>& option(const std::string& option) { _option = option; return *this; }
    param<bool>& name(const std::string& name) { _name = name; return *this; }
    param<bool>& placeholder(const std::string& placeholder) { _placeholder = placeholder; return *this; }
    param<bool>& description(const std::string& description) { _description = description; return *this; }
    param<bool>& prefix(const std::string& prefix) { _prefix = prefix; return *this; }
    param<bool>& required(bool req) { _required = req; return *this; }

    bool required() const { return _required; }
    bool optional() const { return !_required; }
    bool valid() const    { return _valid; }
    bool is_set() const   { return _is_set; }

    void read(std::stringstream& stream){
        stream >> _value;
        _valid = ((stream.rdstate() & std::ios::failbit) == 0);
        _is_set = true;
    }

    /**
     * @brief Assuming that the key has been found, now parse the value
     * 
     * @tparam StrIt 
     * @tparam VectorIt 
     * @param v_it position of the vector on which the key of the param is found
     * @param v_end end position of the argument vector
     * @param a_begin begining position of the value of this param
     * @param a_end end position of the argument in which teh key has been found
     * @return VectorIt 
     */
    template <typename StrIt, typename VectorIt>
    VectorIt parse(VectorIt v_it, VectorIt, StrIt, StrIt){
        _is_set = true;
        _valid  = true;
        _value  = true;
        return v_it+1;
    }
    template <typename StreamT>
    StreamT& usage(StreamT& stream) const {
        if(optional()) stream << "[ ";
        if(!_prefix.empty()){
            if(!_option.empty()){
                stream << _prefix << _option;
            }else if(!_name.empty()){
                stream << _prefix << _prefix << _name;
            }
        }
        if(optional()) stream << " ]";
        return stream;
    }
    template <typename StreamT>
    StreamT& desc(StreamT& stream) const {
        std::size_t chars_written = 0;
        if(!_prefix.empty()){
            if(!_option.empty()){
                stream << _prefix << _option;
                chars_written += _prefix.size() + _option.size();
            }
            if(!_option.empty() && !_name.empty()){
                stream << ", ";
                chars_written += 2;
            }
            if(!_name.empty()){
                stream << _prefix << _prefix << _name;
                chars_written += 2*_prefix.size() + _name.size();
            }
        }
        if(chars_written < 25){
            std::string space(25-chars_written, ' ');
            stream << space;
        }
        stream << " " << _description;
        return stream;
    }
    private:
        bool& _value;
        bool _is_set;
        bool _required;
        bool _valid;
        std::string _prefix;
        std::string _option;
        std::string _name;
        std::string _placeholder;
        std::string _description;
};

}
}

#endif // DHYARA_CMD_PARAM_H
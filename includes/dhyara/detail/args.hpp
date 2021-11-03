/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_ARGS_H
#define DHYARA_ARGS_H

#include <algorithm>
#include <iterator>
#include <string>
#include <sstream>

namespace dhyara{
/**
 * @brief 
 * @code
    using namespace dhyara::cmd;
    
    int input = 0;
    param<int> p = arg("-i", "--input") & value("N", input) % "input description";
    chain<param<int>> chain(p);
    
    std::vector<std::string> args = {"--input", "5"};
    chain.parse(args.begin(), args.end());
    
    std::cout << input << std::endl;
 * @endcode 
 */
namespace cmd{

namespace detail{
    template<class ForwardIt1, class ForwardIt2>
    constexpr ForwardIt1 search(ForwardIt1 first, ForwardIt1 last, ForwardIt2 s_first, ForwardIt2 s_last){
        while (1) {
            ForwardIt1 it = first;
            for (ForwardIt2 s_it = s_first; ; ++it, ++s_it) {
                if (s_it == s_last) return first;
                if (it == last)  return last;
                if (!(*it == *s_it)) break;
            }
            ++first;
        }
    }
}

template <typename T>
struct param{
    param(T& value): _value(value) {}
    param(const std::string& option, T& value): _option(option), _value(value) {}
    param(const std::string& option, const std::string& name, T& value): _option(option), _name(name), _value(value) {}
    param(const param<T>&) = default;

    const std::string& option() const { return _option; }
    const std::string& name() const { return _name; }
    const std::string& placeholder() const { return _placeholder; }
    const std::string& description() const { return _description; }

    param<T>& option(const std::string& option) { _option = option; return *this; }
    param<T>& name(const std::string& name) { _name = name; return *this; }
    param<T>& placeholder(const std::string& placeholder) { _placeholder = placeholder; return *this; }
    param<T>& description(const std::string& description) { _description = description; return *this; }

    void read(std::stringstream& stream){
        stream >> _value;
    }

    private:
        T& _value;
        std::string _option;
        std::string _name;
        std::string _placeholder;
        std::string _description;
};

namespace detail{
    template <typename V>
    struct param_value{
        V& _value;
        std::string _placeholder;
        std::string _description;

        param_value(V& value): _value(value) {}
        param_value(const std::string& placeholder, V& value): _value(value), _placeholder(placeholder) {}
        param_value(const param_value<V>&) = default;
    };
    struct param_name{
        std::string _option;
        std::string _name;

        inline explicit param_name(const std::string& option): _option(option) {}
        inline param_name(const std::string& option, const std::string& name): _option(option), _name(name) {}
        inline param_name(const param_name&) = default;
    };
}

inline detail::param_name arg(const std::string& option, const std::string& name = ""){
    return detail::param_name(option, name);
}

template <typename V>
detail::param_value<V> value(V& value){
    return detail::param_value<V>(value);
}
template <typename V>
detail::param_value<V> value(const std::string& placeholder, V& value){
    return detail::param_value<V>(placeholder, value);
}

template <typename V>
param<V> operator&(const detail::param_name& pn, const detail::param_value<V>& pv){
    param<V> p(pn._option, pn._name, pv._value);
    if(!pv._description.empty()){
        p.description(pv._description);
    }
    return p;
}
template <typename V>
param<V>& operator%(param<V>& p, const std::string& desc){
    p.description(desc);
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


template <typename Head, typename Tail = void>
struct chain;

template <typename X, typename Y>
chain<param<X>, chain<param<Y>>> operator,(const param<X>& x, const param<Y>& y){
    return chain<param<X>, chain<param<Y>>>(x, chain<param<Y>>(y));
}

template <typename T, typename Head, typename Tail>
chain<param<T>, chain<Head, Tail>> operator,(const param<T>& head, const chain<Head, Tail>& tail){
    return chain<param<T>, chain<Head, Tail>>(head, tail);
}

namespace detail{
    template <typename T, typename ArgT>
    static auto match_param(const param<T>& p, const ArgT& arg) -> decltype(arg.begin()){
        auto begin = std::begin(arg);
        auto end   = std::end(arg);
        
        // search for the name that starts with double hyphens
        auto it = detail::search(begin, end, std::begin(p.name()), std::end(p.name()));
        if(it != end){
            it = (std::distance(begin, it) == p.name().size()) ? it : end;
        }else{
            // search for the option that starts with single hyphen
            it = detail::search(begin, end, std::begin(p.option()), std::end(p.option()));
            it = (std::distance(begin, it) == p.option().size()) ? it : end;
        }

        return it;
    }
}

template <typename T, typename Head, typename Tail>
struct chain<param<T>, chain<Head, Tail>>{
    using head_type = param<T>;
    using tail_type = chain<Head, Tail>;

    template <typename, typename>
    friend struct chain;

    chain(const head_type& head, const tail_type& tail): _head(head), _tail(tail) {}

    template <typename InputIt>
    InputIt parse(InputIt i, InputIt end){
        if(i == end) return i;

        const auto& arg = *i;
        auto it = detail::match_param(_head, arg);

        if(it != std::end(arg)){
            std::cout << "matched " << arg << std::endl;
            // head parameter matched
            auto rest = std::distance(it, std::end(arg));
            std::stringstream stream;
            if(rest > 1){
                // value provided in head
                std::copy(it, std::end(arg), std::ostream_iterator<char>(stream));
                _head.read(stream);
            }else{
                // value provided in tail
                if(i != end) {
                    stream << *(++i);
                    _tail._head.read(stream);
                    return _tail.skip(++i);
                }else{
                    // TODO error
                }
            }
        }else{
            // head not matched
        }

        return _tail.parse(++i);
    }
    template <typename InputIt>
    InputIt skip(InputIt i, InputIt end){
        if(i == end) return i;
        return _tail.parse(i, end);
    }
    private:
        head_type _head;
        tail_type _tail;
};

template <typename T>
struct chain<param<T>, void>{
    using head_type = param<T>;

    template <typename, typename>
    friend struct chain;

    chain(const head_type& head): _head(head) {}

    template <typename InputIt>
    InputIt parse(InputIt i, InputIt end){
        if(i == end) return i;
        
        const auto& arg = *i;
        auto it = detail::match_param(_head, arg);
        
        std::cout << "head " << _head.name() << " arg " << arg << std::endl;

        if(it != std::end(arg)){
            std::cout << "matched " << arg << std::endl;
            // head parameter matched
            auto rest = std::distance(it, std::end(arg));
            std::stringstream stream;
            if(rest > 1){
                // value provided in head
                std::copy(it, std::end(arg), std::ostream_iterator<char>(stream));
                _head.read(stream);
            }else{
                // value provided in tail
                if(i != end){
                    stream << *(++i);
                }else{
                    // TODO error
                }
            }
        }else{
            // head not matched
        }

        return i;
    }
    private:
        head_type _head;
};

}
}

#endif // DHYARA_ARGS_H
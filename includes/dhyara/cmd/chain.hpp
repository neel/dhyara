/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_CMD_CHAIN_H
#define DHYARA_CMD_CHAIN_H

#include <string>
#include <sstream>
#include <iterator>
#include "fwd.hpp"
#include "param.hpp"
#include "detail.hpp"

namespace dhyara{
namespace cmd{

template <typename T, typename Head, typename Tail>
struct chain<param<T>, chain<Head, Tail>>{
    using head_type = param<T>;
    using tail_type = chain<Head, Tail>;
    
    template <typename X>
    using extend = chain<param<T>, typename tail_type::template extend<X>>;

    template <typename, typename>
    friend struct chain;
    template <typename>
    friend struct detail::chain_parser;

    chain(head_type&& head, tail_type&& tail): _head(std::move(head)), _tail(std::move(tail)) {}
    const head_type& head() const { return _head; }
    const tail_type& tail() const { return _tail; }
    
    template <typename StreamT>
    StreamT& errors(StreamT& stream) const {
        _tail.errors(stream);
        if(_head.required() && !_head.is_set()){
            stream << "no value set for parameter ";
            if(!_head.option().empty()){
                stream << _head.option();
            }else if(!_head.name().empty()){
                stream << _head.name();
            }
            stream << "\n";
        }else if(_head.is_set() && !_head.valid()){
            stream << "invalid value for ";
            if(!_head.option().empty()){
                stream << _head.option();
            }else if(!_head.name().empty()){
                stream << _head.name();
            }
            stream << "\n";
        }
        return stream;
    }
    /**
     * @brief Whether the entire chain is okay or not
     * @note Checks the validity of required parameters only. The optional values are allowed to be invalid
     * @return true 
     * @return false 
     */
    bool okay() const { return _tail.okay() && (_head.valid() || _head.optional()); }
    /**
     * @brief Checks whether the chain is valid or not including the optional parameters
     * @note an optional parameter with no valud set is valid
     * @return true 
     * @return false 
     */
    bool valid() const {
        return _tail.valid() && (
            (_head.is_set()   && _head.valid())   || 
            (_head.optional() && !_head.is_set())
        ); 
    }
    // template <typename X>
    // inline extend<X> upgrade(const param<X>& px) const{
    //     using extended_chain_type = extend<X>;
    //     return extended_chain_type(_head, _tail.template upgrade<X>(px));
    // }
    private:
        template <typename InputIt>
        InputIt _positional(InputIt i){
            if(!_head.prefix().empty() || _head.is_set()){
                return _tail._positional(i);
            }else{
                InputIt next = _tail._positional(i);    // first look for a fit in tail
                if(next == i){                          // not captured in tail, hence capture in head
                    const auto& arg = *i;
                    std::stringstream stream;
                    std::copy(std::begin(arg), std::end(arg), std::ostream_iterator<char>(stream));
                    _head.read(stream);
                    return i+1;
                }
                return next;
            }
        }
        /**
         * \brief parses the argument at position i in the arguments list.
         * Parses the argument at *i. If that matches with the param in this node of the chain then looks for the value in the same argument (e.g. -t20) or advances
         * the iterator to the next argument and tries to use *(i+1) as the value of the param (e.g. -t 20). returns the next of last matched arg's iterator which are 
         * i+1 and i+2 in these two cases. If the param in this node of chain is not matched then calls the _parse method of the tail of the chain.
         * 
         * If no params match with the given arg, then returns i (through the tail)
         * \param i iterator
         * \param end end position
         * \returns the next of last matched arg's iterator or end in case of error
         */
        template <typename InputIt>
        InputIt _parse(InputIt v_it, InputIt v_end){
            if(v_it == v_end) return v_it;
            const auto& arg = *v_it;
            auto arg_end = std::end(arg);
            auto arg_it = detail::match_param(_head, arg);
            if(arg_it != arg_end){
                // head parameter matched
                return _head.parse(v_it, v_end, arg_it+1, arg_end);
            }else{
                // head not matched
                return _tail._parse(v_it, v_end);
            }
            return v_end;
        }

        head_type _head;
        tail_type _tail;
};

template <typename T>
struct chain<param<T>, void>{
    using head_type = param<T>;
    template <typename X>
    using extend = chain<param<T>, chain<param<X>>>;

    template <typename, typename>
    friend struct chain;
    template <typename>
    friend struct detail::chain_parser;

    chain(head_type&& head): _head(std::move(head)) {}
    const head_type& head() const { return _head; }

    bool okay() const { return _head.valid() || _head.optional(); }
    bool valid() const {
        return (
            (_head.is_set()   && _head.valid())   || 
            (_head.optional() && !_head.is_set())
        ); 
    }
    template <typename StreamT>
    StreamT& errors(StreamT& stream) const {
        if(_head.required() && !_head.is_set()){
            stream << "no value set for parameter ";
            if(!_head.option().empty()){
                stream << _head.option();
            }else if(!_head.name().empty()){
                stream << _head.name();
            }
        }else if(_head.is_set() && !_head.valid()){
            stream << "invalid value for ";
            if(!_head.option().empty()){
                stream << _head.option();
            }else if(!_head.name().empty()){
                stream << _head.name();
            }
            stream << " " << _head.value();
        }
        return stream;
    }
    // template <typename X>
    // inline extend<X> upgrade(const param<X>& px) const{
    //     using extended_chain_type = extend<X>;
    //     return extended_chain_type(_head, typename extended_chain_type::tail_type(px));
    // }
    
    private:
        template <typename InputIt>
        InputIt _positional(InputIt i){
            if(!_head.prefix().empty() || _head.is_set()){
                return i; // Not matched or already matched
            }else{
                const auto& arg = *i;
                std::stringstream stream;
                std::copy(std::begin(arg), std::end(arg), std::ostream_iterator<char>(stream));
                _head.read(stream);
                return i+1;
            }
        }
        /**
         * \brief parses the argument at position i in the arguments list..
         * Parses the argument at *i. If that matches with the param in this node of the chain then looks for the value in the same argument (e.g. -t20) or advances
         * the iterator to the next argument and tries to use *(i+1) as the value of the param (e.g. -t 20). returns the next of last matched arg's iterator which are 
         * i+1 and i+2 in these two cases. If the param in this node of chain is not matched then returns i.
         * \param i iterator
         * \param end end position
         * \returns the next of last matched arg's iterator or end in case of error
         */
        template <typename InputIt>
        InputIt _parse(InputIt v_it, InputIt v_end){
            if(v_it == v_end) return v_it;
            const auto& arg = *v_it;
            auto arg_end = std::end(arg);
            auto arg_it = detail::match_param(_head, arg);
            if(arg_it != arg_end){
                // head parameter matched
                return _head.parse(v_it, v_end, arg_it+1, arg_end);
            }else{
                // head not matched
                return v_it;
            }
            return v_end;
        }

        head_type _head;
};

template <>
struct chain<void, void>{
    template <typename, typename>
    friend struct chain;
    template <typename>
    friend struct detail::chain_parser;

    template <typename X>
    using extend = chain<param<X>, void>;
    template <typename X>
    // inline extend<X> upgrade(const param<X>& px) const{
    //     return extend<X>(px);
    // }

    bool okay() const { return true; }
    bool valid() const { return true; }

    template <typename StreamT>
    StreamT& errors(StreamT& stream) const {
        return stream;
    }
};

using none = chain<void, void>;

namespace detail{
    template <typename Head, typename... Rest>
    struct args{
        using type = chain<param<Head>, typename detail::args<Rest...>::type>;
    };
    template <typename Head>
    struct args<Head>{
        using type = chain<param<Head>>;
    };
}

template <typename... X>
using args = typename detail::args<X...>::type;

}
}

#endif // DHYARA_CMD_CHAIN_H
/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_CMD_PARSE_H
#define DHYARA_CMD_PARSE_H

#include "fwd.hpp"
#include <string>
#include <iterator>

namespace dhyara{
namespace cmd{

namespace detail{
    template <typename Head, typename Tail>
    struct chain_parser<chain<Head, Tail>>{
        chain_parser(chain<Head, Tail>& chain): _chain(chain) {}

        template <typename InputIt>
        InputIt parse(InputIt v_begin, InputIt v_end){
            InputIt it = v_begin;
            InputIt next = it;
            while(it != v_end){
                next = _chain._parse(it, v_end);
                if(next == it){
                    next = _chain._positional(it);
                    it = (next == it) ? next +1 : next;
                }else{
                    it = next;
                }
            }
            return it;
        }

        private:
            chain<Head, Tail>& _chain;
    };
}

template <typename Head, typename Tail, typename InputIt>
InputIt parse(chain<Head, Tail>& chain, InputIt v_begin, InputIt v_end){
    using chain_type = cmd::chain<Head, Tail>;
    detail::chain_parser<chain_type> parser(chain);
    return parser.parse(v_begin, v_end);
}

}
}

#endif // DHYARA_CMD_PARSE_H
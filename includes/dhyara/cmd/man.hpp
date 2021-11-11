/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_CMD_MAN_H
#define DHYARA_CMD_MAN_H

#include "fwd.hpp"
#include <string>

namespace dhyara{
namespace cmd{

namespace detail{
    template <typename Head, typename Tail>
    struct man<chain<Head, Tail>>{
        man(const chain<Head, Tail>& chain): _chain(chain) {}
        template <typename StreamT>
        StreamT& usage(StreamT& stream) const {
            detail::man<Tail> tail_man(_chain.tail());
            tail_man.usage(stream);
            stream << " ";
            _chain.head().usage(stream);
            return stream;
        }
        template <typename StreamT>
        StreamT& desc(StreamT& stream) const {
            detail::man<Tail> tail_man(_chain.tail());
            tail_man.desc(stream);
            stream << "\n";
            stream << "\t";
            _chain.head().desc(stream);
            return stream;
        }
        private:
            const chain<Head, Tail>& _chain;
    };

    template <typename Head>
    struct man<chain<Head, void>>{
        man(const chain<Head, void>& chain): _chain(chain) {}
        template <typename StreamT>
        StreamT& usage(StreamT& stream) const {
            return _chain.head().usage(stream);
        }
        template <typename StreamT>
        StreamT& desc(StreamT& stream) const {
            stream << "\t";
            return _chain.head().desc(stream);
        }
        private:
            const chain<Head, void>& _chain;
    };
}

template <typename StreamT, typename Head, typename Tail>
StreamT& man(const chain<Head, Tail>& chain, const std::string& name, StreamT& stream){
    using chain_type = cmd::chain<Head, Tail>;
    detail::man<chain_type> man_helper(chain);
    stream << "SYNOPSIS: " << "\n";
    stream << "\t" << name << " ";
    man_helper.usage(stream);
    stream << "\n";
    stream << "OPTIONS: " << "\n";
    man_helper.desc(stream);
    stream << "\n";
    return stream;
}

}
}

#endif // DHYARA_CMD_MAN_H
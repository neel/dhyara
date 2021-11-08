/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_CMD_FWD_H
#define DHYARA_CMD_FWD_H

namespace dhyara{
namespace cmd{

template <typename T>
struct param;

template <typename Head, typename Tail = void>
struct chain;

namespace detail{
    struct param_name;

    template <typename V>
    struct param_value;

    template <typename ChainT>
    struct chain_parser;

    template <typename ChainT>
    struct man;
}

}
}

#endif // DHYARA_CMD_FWD_H
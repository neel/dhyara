/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_DETAIL_ARGS_HELPER_H
#define DHYARA_DETAIL_ARGS_HELPER_H

#include <set>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <iterator>

namespace dhyara {
namespace detail{

template <typename InputIt, typename T, typename ContainerT>
InputIt split(InputIt begin, InputIt end, const T& value, ContainerT& container){
    using difference_type = typename std::iterator_traits<InputIt>::difference_type;
    using value_type = typename ContainerT::value_type;
    
    auto inserter = std::inserter(container, container.end());
    
    InputIt last = begin, i = last;
    for(; ((i = std::find(last, end, value)), last) != end; last = (i == end) ? i : i+1){
        difference_type length = std::distance(last, i);
        if(length < 1 || *last == 0x0) continue;
        value_type arg;
        std::copy(last, i, std::back_inserter(arg));
        *inserter++ = arg;
    }
    return i;
}

template <typename InputIt, typename ContainerT>
InputIt split_space(InputIt begin, InputIt end, ContainerT& container){
    return split(begin, end, ' ', container);
}

template <typename InputIt, typename ContainerT>
InputIt read_args(InputIt begin, InputIt end, ContainerT& container){
    return split_space(begin, end, container);
}

template <typename InputIt>
std::vector<std::basic_string<typename std::iterator_traits<InputIt>::value_type>> read_args(InputIt begin, InputIt end){
    std::vector<std::basic_string<typename std::iterator_traits<InputIt>::value_type>> container;
    read_args(begin, end, container);
    return container;
}

template <typename CharT>
std::vector<std::basic_string<CharT>> read_args(const std::basic_string<CharT>& str){
    std::vector<std::basic_string<CharT>> container;
    read_args(str.begin(), str.end(), container);
    return container;
}

template <typename CharT>
std::vector<std::basic_string<CharT>> read_args(const CharT* str){
    std::vector<std::basic_string<CharT>> container;
    read_args(str, str + std::strlen(str), container);
    return container;
}

}
}

#endif // DHYARA_DETAIL_ARGS_HELPER_H
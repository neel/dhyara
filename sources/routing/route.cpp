/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/routing/route.h"
#include <sstream>

dhyara::routing::route::route(const dhyara::address& dst, const dhyara::address& via): _dst(dst), _via(via){}

bool dhyara::routing::route::operator<(const dhyara::routing::route& other) const{
    return std::tie(_dst, _via) < std::tie(other._dst, other._via);
}

bool dhyara::routing::route::operator==(const dhyara::routing::route& other) const{
    return _dst == other._dst && _via == other._via;
}

std::string dhyara::routing::route::to_string() const{
    std::stringstream stream;
    stream << *this;
    return stream.str();
}

std::ostream& dhyara::routing::operator<<(std::ostream& os, const dhyara::routing::route& route){
    os << "Route< "<< route.dst() << " via " << route.via() << " >";
    return os;
}

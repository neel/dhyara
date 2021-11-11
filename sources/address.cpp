/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/address.h"
#include <cstring>

dhyara::address::address(const dhyara::address::container_type& addr): _bytes(addr), _valid(true){}
dhyara::address::address(std::uint8_t b1, std::uint8_t b2, std::uint8_t b3, std::uint8_t b4, std::uint8_t b5, std::uint8_t b6): _valid(true){
    _bytes[0] = b1;
    _bytes[1] = b2;
    _bytes[2] = b3;
    _bytes[3] = b4;
    _bytes[4] = b5;
    _bytes[5] = b6;
}
dhyara::address::address(const std::uint8_t* bytes): _valid(true){
    std::copy(bytes, bytes + 6, _bytes.begin());
}

dhyara::address::address(const std::string& addr){
    std::uint32_t b[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    if (std::sscanf(addr.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x", &b[0], &b[1], &b[2], &b[3], &b[4], &b[5]) != 6){
        std::cerr << "Invalid Address" << std::endl;
        _valid = false;
    }else{
        std::copy(b, b+6, _bytes.begin());
        _valid = true;
    }
    
}

std::string dhyara::address::to_string() const{
    char buff[18];
    std::memset(buff, 0, sizeof(buff));
    snprintf(buff, sizeof(buff), "%02x:%02x:%02x:%02x:%02x:%02x", _bytes[0], _bytes[1], _bytes[2], _bytes[3], _bytes[4], _bytes[5]);
    return std::string(buff);
}


std::uint64_t dhyara::address::hash() const{
    return
        std::uint64_t(_bytes[0]) << 40 |
        std::uint64_t(_bytes[1]) << 32 | ( 
            std::uint32_t(_bytes[2]) << 24 | 
            std::uint32_t(_bytes[3]) << 16 |
            std::uint32_t(_bytes[4]) << 8 |
            std::uint32_t(_bytes[5])
        );
}
bool dhyara::address::operator<(const dhyara::address& other) const{
    return hash() < other.hash();
}
bool dhyara::address::operator==(const dhyara::address& other) const{
    return (b1() == other.b1())
        && (b2() == other.b2())
        && (b3() == other.b3())
        && (b4() == other.b4())
        && (b5() == other.b5())
        && (b6() == other.b6());
}

bool dhyara::address::operator==(const std::string& addr) const{
    return operator==(dhyara::address(addr));
}

void dhyara::address::set(const std::uint8_t* bytes){
    std::copy(bytes, bytes + 6, _bytes.begin());
}

std::ostream& dhyara::operator<<(std::ostream& os, const dhyara::address& address){
    os << "Addr< " << address.to_string() << " >";
    return os;
}

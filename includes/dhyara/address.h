/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_ADDRESS_H
#define DHYARA_ADDRESS_H

#include <set>
#include <array>
#include <string>
#include <vector>
#include <cstdint>
#include <numeric>
#include <iostream>
#include <algorithm>
#include <functional>

namespace dhyara{

/**
 * peer address (mac address)
 * \ingroup dhyara
 */
struct address{
    typedef std::array<std::uint8_t, 6> container_type;
    
    /**
     * construct mac address by an array
     */
    explicit address(const container_type& addr);
    /**
     * construct mac address by octets
     */
    address(std::uint8_t b1, std::uint8_t b2, std::uint8_t b3, std::uint8_t b4, std::uint8_t b5, std::uint8_t b6);
    /**
     * construct mac address by providing address to an array
     */
    explicit address(const std::uint8_t* bytes);
    /**
     * construct mac address by string
     */
    explicit address(const std::string& addr);
    
    /**
     * null mac
     */
    inline static address null() { return address(0x0, 0x0, 0x0, 0x0, 0x0, 0x0); }
    /**
     * broadcast mac
     */
    inline static address all() { return address(0xff, 0xff, 0xff, 0xff, 0xff, 0xff); }
    
    /**
     * convert to string
     */
    std::string to_string() const;
    /**
     * raw array
     */
    inline const std::uint8_t* raw() const {return _bytes.data();}
    /**
     * first octet of the mac address
     */
    inline std::uint8_t b1() const {return _bytes[0];}
    /**
     * second octet of the mac address
     */
    inline std::uint8_t b2() const {return _bytes[1];}
    /**
     * third octet of the mac address
     */
    inline std::uint8_t b3() const {return _bytes[2];}
    /**
     * fourth octet of the mac address
     */
    inline std::uint8_t b4() const {return _bytes[3];}
    /**
     * fifth octet of the mac address
     */
    inline std::uint8_t b5() const {return _bytes[4];}
    /**
     * sixth octed  of the mac address
     */
    inline std::uint8_t b6() const {return _bytes[5];}
    /**
     * 64 bit hash value for the mac address
     */
    std::uint64_t hash() const;
    /**
     * Less than comparator
     */
    bool operator<(const address& other) const;
    /**
     * Comparator
     */
    bool operator==(const address& other) const;
    /**
     * Compare with string
     */
    bool operator==(const std::string& addr) const;
    /**
     * Comparator
     */
    inline bool operator!=(const address& other) const { return !(*this == other); }
    /**
     * Comparator
     */
    inline bool operator!=(const std::string& addr) const { return !(*this == addr); }
    
    /**
     * Set the octets directly
     */
    void set(const std::uint8_t* bytes);
    
    /**
     * Copy the MAC address to another container by bytes
     */
    template <typename OutIt>
    OutIt copy(OutIt it) const{
        return std::copy(_bytes.begin(), _bytes.end(), it);
    }
    
    /**
     * check whether the mac address is null 
     */
    inline bool is_null() const { return std::accumulate(_bytes.begin(), _bytes.end(), 0) == 0; }
    /**
     * check whether the mac address is broadcast
     */
    inline bool is_broadcast() const { return std::all_of(_bytes.begin(), _bytes.end(), std::bind(std::equal_to<std::uint8_t>(), std::placeholders::_1, 0xff)); }
    /**
     * whether the mac address is valid
     */
    inline bool valid() const { return _valid; }
    private:
        container_type _bytes;
        bool           _valid;
};

std::ostream& operator<<(std::ostream& os, const dhyara::address& address);

}

namespace std {

template <>
struct hash<dhyara::address>{
    std::uint64_t operator()(const dhyara::address& addr) const{
        return std::hash<std::uint64_t>()(addr.hash());
    }
};

}

#endif // DHYARA_ADDRESS_H

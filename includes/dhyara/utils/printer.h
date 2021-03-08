/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_UTILS_PACKET_PRINTER_H
#define DHYARA_UTILS_PACKET_PRINTER_H

#include "dhyara/packets/packets.h"

namespace dhyara{
    
namespace utils{

/**
 * print packets
 * \ingroup packets io
 * \ingroup utils
 */
struct printer{
    void in(const dhyara::peer_address& source, const dhyara::packets::beacon& beacon);
    void in(const dhyara::peer_address& source, const dhyara::packets::acknowledgement& acknowledgement);
    void in(const dhyara::peer_address& source, const dhyara::packets::advertisement& advertisement);
    void in(const dhyara::peer_address& source, const dhyara::packets::chunk& chunk);
    void in(const dhyara::peer_address& source, const dhyara::packets::delivered& delivered);
    void in(const dhyara::peer_address& source, const dhyara::packets::echo_request& echo_request);
    void in(const dhyara::peer_address& source, const dhyara::packets::echo_reply& echo_reply);
    void in(const dhyara::peer_address& source, const dhyara::packets::echo_lost& echo_lost);
    
    void out(const dhyara::peer_address& source, const dhyara::packets::beacon& beacon);
    void out(const dhyara::peer_address& source, const dhyara::packets::acknowledgement& acknowledgement);
    void out(const dhyara::peer_address& source, const dhyara::packets::advertisement& advertisement);
    void out(const dhyara::peer_address& source, const dhyara::packets::chunk& chunk);
    void out(const dhyara::peer_address& source, const dhyara::packets::delivered& delivered);
    void out(const dhyara::peer_address& source, const dhyara::packets::echo_request& echo_request);
    void out(const dhyara::peer_address& source, const dhyara::packets::echo_reply& echo_reply);
    void out(const dhyara::peer_address& source, const dhyara::packets::echo_lost& echo_lost);
};

}

}

#endif // DHYARA_UTILS_PACKET_PRINTER_H

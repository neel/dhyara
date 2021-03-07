/*
 * Copyright (c) 2021, <copyright holder> <email>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY <copyright holder> <email> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> <email> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DHYARA_UTILS_PACKET_PRINTER_H
#define DHYARA_UTILS_PACKET_PRINTER_H

#include "dhyara/packets/packets.h"

namespace dhyara{
    
namespace utils{

/**
 * print packets
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

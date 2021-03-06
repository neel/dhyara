/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_ACTIONS_ADVERTISEMENT_H
#define DHYARA_ACTIONS_ADVERTISEMENT_H

#include "dhyara/peer.h"
#include "dhyara/actions/action.h"
#include "dhyara/packets/advertisement.h"

namespace dhyara{

struct link;
struct synchronizer;
    
namespace actions{
    
/**
 * action to handle an incoming advertisement packet
 * \ingroup actions
 */
struct advertisement: dhyara::actions::action<advertisement, dhyara::packets::advertisement>{
    /**
     * advertisement action constructor requires a link to respond after receiving an advertisement packet. It needs the synchronizer to synchronize its neighbourhood.
     * 
     * \param link reference to the link
     * \param synchronizer reference to the synchronizer 
     */
    inline explicit advertisement(dhyara::link& link, dhyara::synchronizer& synchronizer): _link(link), _synchronizer(synchronizer){}
    
    using action::operator();
    /**
     * The received advertisement is processed in this function.
     * 
     * \param addr source address 
     * \param advertisement the received advertisement 
     */
    void operator()(const dhyara::address& addr, const dhyara::packets::advertisement& advertisement);
    
    private:
        dhyara::link& _link;
        dhyara::synchronizer&  _synchronizer;
};

}

}

#endif // DHYARA_ACTIONS_ADVERTISEMENT_H

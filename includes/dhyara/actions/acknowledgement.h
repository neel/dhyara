/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_ACTIONS_ACKNOWLEDGEMENT_H
#define DHYARA_ACTIONS_ACKNOWLEDGEMENT_H

#include "dhyara/peer.h"
#include "dhyara/actions/action.h"
#include "dhyara/packets/acknowledgement.h"

namespace dhyara{
    
struct link;
struct synchronizer;
    
namespace actions{
    
/**
 * action to handle an incoming acknowledgement packet
 */
struct acknowledgement: dhyara::actions::action<acknowledgement, dhyara::packets::acknowledgement>{
    
    inline explicit acknowledgement(dhyara::link& link, dhyara::synchronizer& synchronizer): _link(link), _synchronizer(synchronizer){}
    
    using action::operator();
    void operator()(const dhyara::peer_address& addr, const dhyara::packets::acknowledgement& acknowledgement);
    
    private:
        dhyara::link& _link;
        dhyara::synchronizer&  _synchronizer;
};

}

}

#endif // DHYARA_ACTIONS_ACKNOWLEDGEMENT_H

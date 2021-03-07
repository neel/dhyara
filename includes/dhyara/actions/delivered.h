/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_ACTIONS_DELIVERED_H
#define DHYARA_ACTIONS_DELIVERED_H

#include "dhyara/peer.h"
#include "dhyara/actions/action.h"
#include "dhyara/packets/delivered.h"

namespace dhyara{
    
struct link;
    
namespace actions{
    
/**
 * action to handle an incoming delivery report
 */
struct delivered: dhyara::actions::action<delivered, dhyara::packets::delivered>{
    
    inline explicit delivered(dhyara::link& link): _link(link){}
    
    using action::operator();
    void operator()(const dhyara::peer_address& addr, const dhyara::packets::delivered& delivered);
    
    private:
        dhyara::link& _link;
};
    
}
    
}

#endif // DHYARA_ACTIONS_DELIVERED_H

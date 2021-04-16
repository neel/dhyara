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
 * \ingroup actions
 */
struct acknowledgement: dhyara::actions::action<acknowledgement, dhyara::packets::acknowledgement>{
    /**
     * acknowledgement action constructor requires a link to respond after receiving an acknowledgement packet. It needs the synchronizer to synchronize its neighbourhood.
     * 
     * \param link reference to the link
     * \param synchronizer reference to the synchronizer 
     */
    inline explicit acknowledgement(dhyara::link& link, dhyara::synchronizer& synchronizer): _link(link), _synchronizer(synchronizer){}
    
    using action::operator();
    
    /**
     * The received acknowledgement is processed in this function.
     * 
     * \param addr source address 
     * \param acknowledgement the received acknowledgement 
     */
    void operator()(const dhyara::peer_address& addr, const dhyara::packets::acknowledgement& acknowledgement);
    
    private:
        dhyara::link& _link;
        dhyara::synchronizer&  _synchronizer;
};

}

}

#endif // DHYARA_ACTIONS_ACKNOWLEDGEMENT_H

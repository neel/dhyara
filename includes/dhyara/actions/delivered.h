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
 * \ingroup actions
 */
struct delivered: dhyara::actions::action<delivered, dhyara::packets::delivered>{
    
    /**
     * A delivery action needs to have access to the link in order to relay the delivery report if it is an intermediate node.
     * 
     * \param link a reference to the link
     */
    inline explicit delivered(dhyara::link& link): _link(link){}
    
    using action::operator();
    /**
     * The received delivery report is processed in this function.
     * 
     * \param addr source address 
     * \param delivered the received delivery report 
     */
    void operator()(const dhyara::address& addr, const dhyara::packets::delivered& delivered);
    
    private:
        dhyara::link& _link;
};
    
}
    
}

#endif // DHYARA_ACTIONS_DELIVERED_H

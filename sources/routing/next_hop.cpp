/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/routing/next_hop.h"

std::ostream& dhyara::routing::operator<<(std::ostream& os, const dhyara::routing::next_hop& hop){
    os << "Hop< " << " via " << hop.via() << ", DELAY " << hop.delay() << " >";
    return os;
}

/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/routing/metric.h"

void dhyara::routing::metric::update(dhyara::delay_type delay, bool sync_updated){
    _delay = delay;
    if(sync_updated){
        update();
    }
}

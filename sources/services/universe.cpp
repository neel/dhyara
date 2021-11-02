/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/services/universe.h"
#include "dhyara/dhyara.h"
#include "dhyara/neighbour.h"
#include "esp_err.h"
#include <iomanip>

esp_err_t dhyara::services::universe::run(dhyara::services::stream &stream){
    dhyara::link& link = dhyara_link();
    bool none_set = (!_only_neighbours && !_only_peers);
    if(none_set || _only_neighbours){
        for(auto it = link.neighbours().begin(); it != link.neighbours().end(); ++it){
            const dhyara::neighbour& neighbour = it->second;
            if(neighbour.addr().is_broadcast()) continue;
            stream  << neighbour.addr().to_string()
                    << " (" << neighbour.name() << ")"
                    << " on channel " << (int)neighbour.channel()
                    << " RSSI " << (int)neighbour.rssi()
                    << "\n";
        }
    }
    if(none_set || (_only_neighbours && _only_peers)){
        stream << "-------------------------------------------------------" << "\n";
    }
    if(none_set || _only_peers){
        for(auto it = link.universe().begin(); it != link.universe().end(); ++it){
            const dhyara::peer& peer = it->second;
            stream << peer.addr().to_string()
                << " (" << peer.name() << ")"
                << "\n";
        }
    }
    stream.finish();
    return ESP_OK;
}
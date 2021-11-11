/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/services/routes.h"
#include "dhyara/dhyara.h"
#include "esp_err.h"
#include <iomanip>

dhyara::cmd::args<std::uint8_t> dhyara::services::routes::options() {
    return (
        cmd::arg("n", "hops") & cmd::value(_hops, "N") % "Only show the routes that are N or more hops away"
    );
}

esp_err_t dhyara::services::routes::run(dhyara::services::stream &stream){
    dhyara::link& link = dhyara_link();
    stream << link.routes().routes_size() << " routes to " << link.routes().next_size() << " destinations" << "\n";
    stream << "--------------------------------------------------------------------" << "\n";

    for(auto it = link.routes().route_begin(); it != link.routes().route_end(); ++it){
        const dhyara::routing::route& route   = it->first;
        const dhyara::routing::metric& metric = it->second;
        if(metric.hops() < _hops) continue;
        stream  << route.dst().to_string() 
                << " via " << route.via().to_string() 
                << " in "<< std::setprecision(2) << std::fixed << ((double)metric.delay()/1000.0) << "ms"
                << " using " << (int)metric.hops() << " hops"
                << " updated on " << metric.updated()
                << "\n";
    }
    stream << "--------------------------------------------------------------------" << "\n";

    for(auto it = link.routes().next_begin(); it != link.routes().next_end(); ++it){
        const dhyara::address& target         = it->first;
        const dhyara::routing::next_hop& best = it->second;
        if(best.hops() < _hops) continue;
        stream  << target.to_string()
                << " via " << best.via().to_string() 
                << " in "<< std::setprecision(2) << std::fixed << ((double)best.delay()/1000.0) << "ms"
                << " using " << (int)best.hops() << " hops"
                << " published on " << best.published()
                << "\n";
    }

    stream.finish();
    return ESP_OK;
}
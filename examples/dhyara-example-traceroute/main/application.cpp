/*
 * Copyright (c) 2021, Neel Basu <neel.basu.z@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY Neel Basu <neel.basu.z@gmail.com> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Neel Basu <neel.basu.z@gmail.com> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "application.h"
#include "esp_log.h"
#include <iostream>
#include <dhyara/tools/ping.h>
#include <dhyara/tools/traceroute.h>

application::application(dhyara::network& network): _network(network), _server(network.link().routes()){}

void application::main(){
    // defining a source and a target in the multi hop network
    dhyara::peer_address target("4c:11:ae:9c:a6:85"), source("4c:11:ae:71:0f:4d");
    
    // For experimental purpose (remove is there are no intermediate nodes)
    // Optionally banning direct communication between the source and the target
    // Forcing the source and target to communicate via one or more intermediate nodes
    if(_network.link().address() == target) _network.beacon().ban(source);
    if(_network.link().address() == source) _network.beacon().ban(target);
    
    while(1){
        // Use traceroute tool to get routes
        dhyara::tools::traceroute traceroute(_network, target);
        // Assuming the same application is running on all nodes
        // The code below should only run in the source node
        if(_network.link().address() == source){
            // run the traceroute tool
            traceroute();
        }
        // sleep for 5s
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
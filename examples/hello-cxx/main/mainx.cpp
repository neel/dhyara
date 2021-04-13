#include "mainx.h"
#include <dhyara/network.h>
#include <iostream>
#include <freertos/FreeRTOS.h>
#include <dhyara/link.h>
#include "esp_wifi.h"
#include <dhyara/dhyara.h>
#include "esp_log.h"
#include <dhyara/tools/ping.h>
#include <dhyara/tools/traceroute.h>
#include <dhyara/packets/data.h>

void mainx(){
    dhyara::network network(dhyara_link());
    const dhyara::peer_address& local = network.link().address();
    
    dhyara::peer_address sink("4c:11:ae:9c:a6:85"), source("4c:11:ae:71:0f:4d");
    
//     For experimental purpose 
//     Optionally ban direct communication between the source and the target
//     Force the source and target to communicate via one or more intermediate nodes
//     network.isolate(source, sink);
    
    network.start();
    
    dhyara::peer_address other = (local == source) ? sink : source;
    
    // The anonymous function will be called once all chunks of a data packet is received
    network.on_data([](const dhyara::peer::address& source, const dhyara::packets::data& data){
        std::cout << "received data " << " originating from " << data.source() << " via " << source << " of size " << data.length() << std::endl;
    });
    
    while(1){
        
        { // ping the other node
            dhyara::tools::ping ping(network);
            ping.count(4).batch(10);
            ping(other);
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
        
        { // traceroute the other node
            dhyara::tools::traceroute traceroute(network, other);
            traceroute();
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
        
        { // send data
            std::string buffer = "Hello World";
            network.send(other, buffer.begin(), buffer.end());
        }
        
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
} 

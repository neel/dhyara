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
#include <dhyara/utils/http.h>

void mainx(){
    dhyara::network network(dhyara_link());
    dhyara_set_default_network(&network);
    
    // dhyara::address sink("ac:67:b2:25:8e:a5"), source("4c:11:ae:9c:1a:c9");
    
    network.start();
    
    /**
     * For experimental purpose 
     * Optionally ban direct communication between the source and the target
     * Force the source and target to communicate via one or more intermediate nodes
     */
    // network.isolate(source, sink);
    
    // dhyara::address local = dhyara_local();
    // dhyara::address other = (local == source) ? sink : ((local == sink) ? source : dhyara::address::null());
    
    /**
     * The anonymous function will be called once all chunks of a data packet is received
     */
    // network.on_data([](const dhyara::address& source, const dhyara::packets::data& data){
    //     std::cout << "received data " << " originating from " << data.source() << " via " << source << " of size " << data.length() << std::endl;
    // });
    
    while(1){
        // if(!other.is_null()){
        //     dhyara::tools::ping ping(network);
        //     ping.count(1).batch(50).sleep(0);
        //     ping(other);
        //     vTaskDelay(pdMS_TO_TICKS(2000));

        //     dhyara::tools::traceroute traceroute(network, other);
        //     traceroute();
        //     vTaskDelay(pdMS_TO_TICKS(2000));

        //     std::string buffer = "Hello World";
        //     network.send(other, buffer.begin(), buffer.end());
        // }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
} 

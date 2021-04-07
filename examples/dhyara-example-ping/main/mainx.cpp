#include "mainx.h"
#include <dhyara/network.h>
#include <iostream>
#include <freertos/FreeRTOS.h>
#include <dhyara/link.h>
#include "esp_wifi.h"
#include "application.h"
#include <dhyara/wifi.h>

void mainx(){
    dhyara_espnow_init();
    dhyara::network network(dhyara_link());
    
//     For experimental purpose 
//     Optionally banning direct communication between the source and the target
//     Forcing the source and target to communicate via one or more intermediate nodes
//     dhyara::peer_address sink("4c:11:ae:9c:a6:85"), source("4c:11:ae:71:0f:4d");
//     network.isolate(source, sink);
    
    network.start();
    
    application app(network);
    app.main();
} 

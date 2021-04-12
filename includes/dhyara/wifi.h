/*
 * Copyright (c) 2021, <copyright holder> <email>
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
 * THIS SOFTWARE IS PROVIDED BY <copyright holder> <email> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> <email> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef DHYARA_WIFI_H
#define DHYARA_WIFI_H

#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"

#ifdef __DOXYGEN__

/**
 * \fn bool dhyara_send(const unsigned char* target, const void* data, unsigned long len)
 * \brief Send a variable length data as data packet to the destination over multi hop network.
 * \param target 6 bytes long mac address
 * \param data pointer to the beginnning of the buffer to send
 * \param len length of the data to be sent (if length is set to zero then uses strlen the get the length of data).
 * \ingroup communication 
 */
bool dhyara_send(const unsigned char* target, const void* data, unsigned long len);

/**
 * \fn bool dhyara_ping(const unsigned char* target, .count = 254, .batch = 1, .sleep = 15)
 * \brief pings target with the provided configuration
 * \param target 6 bytes MAC address
 * \param count number of ICMPQ batches
 * \param batch number of ICMPQ Packets in one batch
 * \param sleep amount of sleep (in ms) between two consecutive batches
 * \return boolean success value
 * \see dhyara::ping 
 * \ingroup tools 
 */
bool dhyara_ping(const unsigned char* target, .count = 254, .batch = 1, .sleep = 15);

#endif 

#ifdef __cplusplus
extern "C" {  
#endif  

extern wifi_promiscuous_filter_t g_dhyara_promiscous_filter;

/**
 * Conveniance function to initialize wifi.
 * \attention esp_wifi_start() should be called after calling this function
 * \attention ESP Now may be initialized (e.g. \ref dhyara_espnow_init) after initializing wifi 
 * \ingroup communication 
 * \param mode wifi mode (AP, STA or APSTA)
 */
esp_err_t dhyara_wifi_init(wifi_mode_t mode);

/**
 * Initialize ESP-Now. Registers the necessary ESP Now send/receive callbacks.
 * \attention should be called after initializing wifi (e.g. \ref dhyara_wifi_init)
 * \ingroup communication 
 */
esp_err_t dhyara_espnow_init();

/**
 * Initializes wifi, starts wifi, then initializes ESP Now 
 * \see dhyara_wifi_init 
 * \see dhyara_espnow_init
 * \ingroup communication
 */
esp_err_t dhyara_init(wifi_mode_t mode);

/**
 * Creates the default global network. It is equivalent to instantiating \ref dhyara::network 
 * 
 * \warning There should be exactly one instance of \ref dhyara::network 
 * \ingroup communication
 */
void dhyara_create_default_network();

/**
 * Starts the default network. Creates a default network if none is set already.
 * 
 * \ingroup communication 
 * \see dhyara_create_default_network 
 */
void dhyara_start_default_network();

/**
 * Checks whether the default network is set or not
 * 
 * \ingroup communication 
 */
bool dhyara_has_default_network();

/**
 * Callback to be called whenever a data packet is arrived.
 * \ingroup communication 
 */
typedef void (*dhyara_data_callback_t) (const unsigned char* source, const void* data, unsigned long len);

/**
 * Sets a callback on the default networks on_data. This callback will be called whenever a data packet is received.
 * 
 * \ingroup communication 
 */
bool dhyara_receive(dhyara_data_callback_t callback);

#ifndef __DOXYGEN__

bool dhyara_send_internal(const unsigned char* target, const void* data, unsigned len);

struct dhyara_send_params{
    unsigned long len;
};

#define dhyara_send(target, data, ...) dhyara_send(target, data, (struct dhyara_send_params){__VA_ARGS__})

inline bool (dhyara_send)(const unsigned char* target, const void* data, struct dhyara_send_params params){
    return dhyara_send_internal(target, data, params.len);
}

bool dhyara_send_ping(const unsigned char* target, uint8_t count, int8_t batch, uint8_t sleep);

#endif

/**
 * Run traceroute on target
 * \param target 6 bytes long mac address
 * \ingroup tools 
 */
bool dhyara_traceroute(const unsigned char* target);

/**
 * gets the mac address of self
 * \param target writable 6 bytes long buffer to hold the mac address
 * \ingroup communication
 */
bool dhyara_local(unsigned char* address);

/**
 * Join to an AP using station interface
 * \ingroup communication 
 * \internal 
 */
esp_err_t dhyara_station_join(wifi_config_t* sta_config);

#ifdef __cplusplus  
} // extern "C"  
#endif

#ifndef __cplusplus

// Visible in C only 

#ifndef __DOXYGEN__
struct dhyara_ping_conf{
    uint8_t count;
    int8_t  batch;
    uint8_t sleep;
};

#define dhyara_ping(target, ...)                                                                        \
    _Pragma("GCC diagnostic push")                                                                      \
    _Pragma("GCC diagnostic ignored \"-Woverride-init\"")                                               \
    dhyara_ping(target, (struct dhyara_ping_conf){.count = 254, .batch = 1, .sleep = 15, __VA_ARGS__}); \
    _Pragma("GCC diagnostic pop")                                                                       \
    do {} while (0)

inline bool (dhyara_ping)(const unsigned char* target, struct dhyara_ping_conf conf){
    return dhyara_send_ping(target, conf.count, conf.batch, conf.sleep);
}
#endif

#endif

#ifdef __cplusplus

// Visible in C++ Only

#include <dhyara/peer.h>

namespace dhyara{
    struct link;
    struct network;
};

/**
 * Get dhyara communication link
 * \ingroup communication 
 */
dhyara::link& dhyara_link();

/**
 * Sets an user created network as default
 * 
 * \param network The network to be set
 * \ingroup communication 
 * 
 * \see dhyara_get_default_network 
 */
void dhyara_set_default_network(dhyara::network* network);

/**
 * Gets the default network set 
 * 
 * \warning ensure that a default network exists before getting it.
 * 
 * \ingroup communication 
 */
dhyara::network* dhyara_get_default_network();

#endif

#endif // DHYARA_WIFI_H

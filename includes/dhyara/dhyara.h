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

// For documentation only

/**
 * \name Send variable length data.
 * 
 * \{
 */
/**
 * \fn bool dhyara_send(const unsigned char* target, const void* data, unsigned long len)
 * \brief Send a variable length data as data packet to the destination over multi hop network.
 * \param target 6 bytes long mac address
 * \param data pointer to the beginnning of the buffer to send
 * \param len length of the data to be sent (if length is set to zero then uses strlen the get the length of data).
 * \ingroup dhyara 
 */
bool dhyara_send(const unsigned char* target, const void* data, unsigned long len);
/**
 * \brief Send a variable length data as data packet to the destination over multi hop network.
 * \param target MAC address of the target
 * \param begin begin iterator 
 * \param end end iterator
 * \ingroup dhyara 
 */
template <typename InputIt>
bool dhyara_send(const dhyara::address& target, InputIt begin, InputIt end);
/**
 * \brief Send a variable length data as data packet to the destination over multi hop network.
 * \param target MAC address of the target
 * \param begin begin iterator 
 * \param len length of the data to be sent (if length is set to zero then uses strlen the get the length of data).
 * \ingroup dhyara 
 */
template <typename InputIt>
bool dhyara_send(const dhyara::address& target, InputIt begin, std::size_t len);
/**
 * \}
 */

/**
 * \name Receive variable length data.
 * 
 * \{
 */
/**
 * Callback for \ref dhyara_receivef.
 * \ingroup dhyara 
 */

/**
 * Sets a callback on the default networks on_data. This callback will be called whenever a data packet is received.
 * The typedef dhyara_receivef_callback_t is defined as the following.
 * \code
 * typedef void (*dhyara_receivef_callback_t) (const unsigned char* source, const void* data, unsigned long len);
 * \endcode
 * Following is an example of the callback function.
 * \code
 * void callback (const unsigned char* source, const void* data, unsigned long len){
 * 
 * }
 * \endcode
 * \param callback The callback function 
 * \ingroup dhyara 
 */
bool dhyara_receivef(dhyara_receivef_callback_t callback);

/**
 * Sets a callback on the default networks on_data. This callback will be called whenever a data packet is received. (accepts C++ anonymous function)
 * The typedef dhyara_receive_callback_t is defined as the following.
 * \code
 * typedef std::function<void (const dhyara::address&, dhyara::packets::data::const_iterator, dhyara::packets::data::const_iterator)> dhyara_receive_callback_type;
 * \endcode
 * Following is an example of the callback function.
 * \code
 * void callback (const dhyara::address&, dhyara::packets::data::const_iterator, dhyara::packets::data::const_iterator){
 * 
 * }
 * \endcode
 * \param callback The callback function
 * \ingroup dhyara 
 */
bool dhyara_receive(dhyara_receive_callback_type callback);
/**
 * Sets a callback on the default networks on_data. This callback will be called whenever a data packet is received. (accepts C++ anonymous function)
 * The typedef dhyara_receive_data_callback_type is defined as the following.
 * \code
 * typedef std::function<void (const dhyara::address&, const dhyara::packets::data& data)> dhyara_receive_data_callback_type;
 * \endcode
 * Following is an example of the callback function.
 * \code
 * void callback (const dhyara::address&, const dhyara::packets::data& data){
 * 
 * }
 * \endcode
 * \param callback The callback function
 * \ingroup dhyara 
 */
bool dhyara_receive_data(dhyara_receive_data_callback_type callback);
/**
 * \}
 */

/**
 * \name Ping
 * \{
 */

/**
 * pings target with the configuration provided as designated initializers
 * \code
 * uint8_t sink[] = {0x4c, 0x11, 0xae, 0x9c, 0xa6, 0x85};
 * dhyara_ping(sink, .count = 1, .batch = 10);
 * \endcode
 * \param target target MAC address
 * \param count number of ECHOQ batches
 * \param batch number of ECHOQ Packets in one batch
 * \param sleep amount of sleep (in ms) between two consecutive batches
 * \attention Only available in C
 * \return boolean success value
 * \see dhyara::ping 
 * \ingroup tools 
 */
bool dhyara_ping(const unsigned char* target, count = 254, batch = 1, sleep = 15);
/**
 * pings target with the provided configuration
 * \code
 * dhyara::address sink("4c:11:ae:9c:a6:85");
 * dhyara_ping(other, 1, 10);
 * \endcode
 * \param target target MAC address
 * \param count number of ECHOQ batches
 * \param batch number of ECHOQ Packets in one batch
 * \param sleep amount of sleep (in ms) between two consecutive batches
 * \return boolean success value
 * \see dhyara::ping 
 * \ingroup tools 
 */
bool dhyara_ping(const dhyara::address& target, uint8_t count = 254, int8_t batch = 1, uint8_t sleep = 15);

/**
 * \}
 */

/**
 * \name Traceroute
 * \{
 */
/**
 * Run traceroute on target
 * \param target 6 bytes long mac address
 * \ingroup tools 
 */
bool dhyara_traceroute(const unsigned char* target);
/**
 * Run traceroute on target (C++ overload)
 * \param target MAC address of the destination
 * \ingroup tools 
 */
bool dhyara_traceroute(const dhyara::address& target);
/**
 * \}
 */

/**
 * \name Local
 * \{
 */
/**
 * gets the mac address of self
 * \param address writable 6 bytes long buffer to hold the mac address
 * \ingroup dhyara
 */
bool dhyara_local(unsigned char* address);
/**
 * gets the mac address of self
 * \ingroup dhyara
 */
dhyara::address dhyara_local();
/**
 * \}
 */


#endif 

#ifdef __cplusplus
extern "C" {  
#endif  

// Available in both C and C++
    
extern wifi_promiscuous_filter_t g_dhyara_promiscous_filter;

/**
 * Conveniance function to initialize wifi.
 * \attention esp_wifi_start() should be called after calling this function
 * \attention ESP Now may be initialized (e.g. \ref dhyara_espnow_init) after initializing wifi 
 * \ingroup dhyara 
 * \param mode wifi mode (AP, STA or APSTA)
 */
esp_err_t dhyara_wifi_init(wifi_mode_t mode);

/**
 * Conveniance function to start wifi.
 * \attention esp_wifi_start() is called in this function
 * \attention \ref dhyara_wifi_init should be called before calling this function
 * \attention ESP Now may be initialized (e.g. \ref dhyara_espnow_init) after initializing and starting the wifi 
 * \ingroup dhyara 
 * \param mode wifi mode (AP, STA or APSTA)
 */
esp_err_t dhyara_wifi_start(wifi_mode_t mode);

/**
 * Initialize ESP-Now. Registers the necessary ESP Now send/receive callbacks.
 * \attention should be called after initializing wifi (e.g. \ref dhyara_wifi_init)
 * \ingroup dhyara 
 */
esp_err_t dhyara_espnow_init();

/**
 * Initializes wifi, starts wifi, then initializes ESP Now 
 * \see dhyara_wifi_init 
 * \see dhyara_wifi_start
 * \see dhyara_espnow_init
 * \ingroup dhyara
 */
esp_err_t dhyara_init(wifi_mode_t mode);

/**
 * Creates the default global network. It is equivalent to instantiating \ref dhyara::network.
 * \attention User code may not call this function directly. Use \ref dhyara_start_default_network instead.
 * \warning There should be exactly one instance of \ref dhyara::network 
 * \ingroup dhyara
 */
void dhyara_create_default_network();

/**
 * Starts the default network. Creates a default network if none is set already.
 * 
 * \ingroup dhyara 
 * \see dhyara_create_default_network 
 */
void dhyara_start_default_network();

/**
 * Checks whether the default network is set or not
 * 
 * \ingroup dhyara 
 */
bool dhyara_has_default_network();

/**
 * @brief C conveniance function to isolate two nodes x and y using the default network
 * \note calls network::isolate C++ function.
 * \attention requires the default network to be set.
 * @param x MAC address of node x
 * @param y MAC address of node y
 * @return returns success value. returns false if it does not already have a default network set.
 * \ingroup dhyara 
 */
bool dhyara_isolate(const unsigned char* x, const unsigned char* y);

#ifndef __DOXYGEN__

typedef void (*dhyara_receivef_callback_t) (const unsigned char*, const void*, unsigned long);
bool dhyara_receivef(dhyara_receivef_callback_t callback);

/**
 * Join to an AP using station interface
 * \ingroup dhyara 
 * \internal 
 */
esp_err_t dhyara_station_join(wifi_config_t* sta_config);

bool dhyara_traceroute(const unsigned char* target);

bool dhyara_local(unsigned char* address);

bool dhyara_send_ping(const unsigned char* target, uint8_t count, int8_t batch, uint8_t sleep);
bool dhyara_send_internal(const unsigned char* target, const void* data, unsigned len);

#endif // __DOXYGEN__

#ifdef __cplusplus  
} // extern "C"  
#endif

#ifndef __cplusplus

// Visible in C only 

#ifndef __DOXYGEN__

struct dhyara_send_params{
    unsigned long len;
};

#define dhyara_send(target, data, ...) dhyara_send(target, data, (struct dhyara_send_params){__VA_ARGS__})

inline bool (dhyara_send)(const unsigned char* target, const void* data, struct dhyara_send_params params){
    return dhyara_send_internal(target, data, params.len);
}

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
#endif // __DOXYGEN__

#endif // __cplusplus

#ifdef __cplusplus

// Visible in C++ Only

#include <functional>
#include <dhyara/peer.h>
#include <dhyara/link.h>
#include <dhyara/network.h>

/**
 * Get dhyara communication link
 * \ingroup dhyara 
 */
dhyara::link& dhyara_link();

/**
 * Sets an user created network as default
 * 
 * \param network The network to be set
 * \ingroup dhyara 
 * 
 * \see dhyara_get_default_network 
 */
void dhyara_set_default_network(dhyara::network* network);

/**
 * Gets a pointer to the default network. 
 * 
 * \ingroup dhyara 
 */
dhyara::network* dhyara_get_default_network();

/**
 * Gets a reference to the default network (assuming it is already set). 
 * 
 * \warning To ensure that a default network exists before getting it.
 * 
 * \ingroup dhyara 
 * \see dhyara_has_default_network 
 */
dhyara::network& dhyara_default_network();

#ifndef __DOXYGEN__

dhyara::address dhyara_local();

bool dhyara_ping(const dhyara::address& target, uint8_t count = 254, int8_t batch = 1, uint8_t sleep = 15);

bool dhyara_traceroute(const dhyara::address& target);

template <typename InputIt>
bool dhyara_send(const dhyara::address& target, InputIt begin, InputIt end){
    return dhyara_default_network().send(target, begin, end);
}

template <typename InputIt>
bool dhyara_send(const dhyara::address& target, InputIt begin, std::size_t len){
    return dhyara_default_network().send(target, begin, len);
}

typedef std::function<void (const dhyara::address&, dhyara::packets::data::const_iterator, dhyara::packets::data::const_iterator)> dhyara_receive_callback_type;
typedef std::function<void (const dhyara::address&, const dhyara::packets::data& data)> dhyara_receive_data_callback_type;
bool dhyara_receive(dhyara_receive_callback_type callback);
bool dhyara_receive_data(dhyara_receive_data_callback_type callback);

#endif // __DOXYGEN__

#endif // __cplusplus

#endif // DHYARA_WIFI_H

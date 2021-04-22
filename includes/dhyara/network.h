/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_NETWORK_H
#define DHYARA_NETWORK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "dhyara/link.h"
#include "dhyara/actions.h"
#include "dhyara/synchronizer.h"
#include "dhyara/xqueue.h"

#if DHYARA_ENABLED_HTTP_MANAGEMENT_SERVER
#include "dhyara/utils/http.h"
#endif

namespace dhyara{

/**
 * The dhyara ad-hoc network. The network is started using the \ref start method. All related networking tasks and actions are created and maintained by the network.
 * \ingroup dhyara
 */
struct network{
    /**
     * Not default constructible
     */
    network() = delete;
    /**
     * Not copyable
     */
    network(const network&) = delete;
    /**
     * Not Copyable
     **/
    network& operator=(const network&) = delete;
    
    /**
     * Construct a network with a reference to the link
     */
    network(dhyara::link& link);
    
    /**
     * main network I/O
     */
    void start();
    
    /**
     * Send a variable sized data to the destination. The data may be splitted into multiple chunks if its not accomodable in a single packet. Any forward iterable sequence, that can be dereferenced as a byte can be considered ad data.
     * 
     * \param target target mac address 
     * \param begin begin iterator to the data that has to be sent
     * \param end end iterator to the data that that has to be sent
     */
    template <typename InputIt>
    bool send(const dhyara::address& target, InputIt begin, InputIt end){
        return send(target, dhyara::packets::data(target, begin, end));
    }
    
    /**
     * Send a variable sized data to the destination. The data may be splitted into multiple chunks if its not accomodable in a single packet. Any forward iterable sequence, that can be dereferenced as a byte can be considered ad data.
     * 
     * \param target target mac address 
     * \param begin begin iterator to the data that has to be sent
     * \param count number of elements to send
     */
    template <typename InputIt>
    bool send(const dhyara::address& target, InputIt begin, std::size_t count){
        return send(target, begin, begin+count);
    }
    
    /**
     * Send a data packet
     * \see dhyara::packets::data
     */
    bool send(const dhyara::address& target, const dhyara::packets::data& data);
    
    public:
    
        /**
         * Isolate two nodes from each other. 
         * The isolated nodes won't be able to communicate with each other directly. 
         * However they will be able to communicate via intermediate nodes. 
         * This function can be used to force multi-hop communication between two nodes even when they can communicate directly.
         * 
         * \attention isolation is achieved by banning x from acknowledgeing y's beacon and banning y from acknowledging y's beacon.
         * \attention It is desirable but not necessary to perform isolation before the network is started.
         * 
         * \param x the first node
         * \param y the second node
         */
        void isolate(const dhyara::address& x, const dhyara::address& y);
        
    public:
        /**
        * set a callback which will be called once a complete data is received
        */
        void on_data(dhyara::actions::chunk::data_callback_type slot);
    
    private:
    
        /**
         * Send presense beacon. Depreciates inactive routes.
         */
        void presence();
        /**
         * Synchronize neighbourhood
         */
        void sync();
        
    public:
        /**
         * Link 
         */
        inline dhyara::link& link() { return _link; }
        /**
         * \name Actions
         */
        /**
         * synchronizer
         */
        inline dhyara::synchronizer& synchronizer() { return _synchronizer; }
        /**
         * beacon action
         **/
        inline dhyara::actions::beacon& beacon() { return _beacon;}
        /**
         * acknowledgement action
         */
        inline dhyara::actions::acknowledgement& acknowledgement() { return _acknowledgement; }
        /**
         * advertisement action
         */
        inline dhyara::actions::advertisement& advertisement() { return _advertisement; }
        /**
         * echo_request action
         */
        inline dhyara::actions::echo_request& echo_request() { return _echo_request; }
        /**
         * echo_reply action
         */
        inline dhyara::actions::echo_reply& echo_reply() { return _echo_reply; }
        /**
         * echo_lost action
         */
        inline dhyara::actions::echo_lost& echo_lost() { return _echo_lost; }
        /**
         * chunk action
         */
        inline dhyara::actions::chunk& chunk() { return _chunk; }
        /**
         * delivered action
         */
        inline dhyara::actions::delivered& delivered() { return _delivered; }
        /**
         * \}
         */
    
    private:
        dhyara::link&                    _link;
        dhyara::synchronizer             _synchronizer;
        dhyara::actions::beacon          _beacon;
        dhyara::actions::acknowledgement _acknowledgement;
        dhyara::actions::advertisement   _advertisement;
        dhyara::actions::echo_request    _echo_request;
        dhyara::actions::echo_reply      _echo_reply;
        dhyara::actions::echo_lost       _echo_lost;
        dhyara::actions::chunk           _chunk;
        dhyara::actions::delivered       _delivered;
        
#if DHYARA_ENABLED_HTTP_MANAGEMENT_SERVER
        dhyara::utils::http              _server;
#endif
        
    private:
        /**
         * Presence Task
         */
        static void task_presence(void* arg);
        /**
         * Neighbourhood synchronization task
         */
        static void task_synchronize(void* arg);
        /**
         * Receive Task
         */
        static void task_start_rcvd(void* arg);
        
#if DHYARA_ENABLED_SEND_QUEUEING
        /**
         * Send Task
         */
        static void task_start_send(void* arg);
#endif
        
#if DHYARA_ENABLED_HTTP_MANAGEMENT_SERVER
    public:
        inline dhyara::utils::http& management_server() { return _server; }
#endif
};

}

#endif // DHYARA_NETWORK_H

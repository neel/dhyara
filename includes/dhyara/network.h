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

namespace dhyara{

/**
 * The dhyara ad-hoc network.
 * The network is started using the \ref start method, that creates all related networking tasks.
 */
struct network{
    network() = delete;
    network(const network&) = delete;
    network& operator=(const network&) = delete;
    
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
    bool send(const dhyara::peer_address& target, InputIt begin, InputIt end){
        return send(target, dhyara::packets::data(target, begin, end));
    }
    
    /**
     * Send a data packet
     * \see dhyara::packets::data
     */
    bool send(const dhyara::peer_address& target, const dhyara::packets::data& data);
    
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
        
    private:
        static void task_presence(void* arg);
        static void task_synchronize(void* arg);
        static void task_start_rcvd(void* arg);
};

}

#endif // DHYARA_NETWORK_H

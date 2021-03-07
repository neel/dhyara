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
    
    template <typename InputIt>
    bool send(const dhyara::peer_address& target, InputIt begin, InputIt end){
        return send(target, dhyara::packets::data(target, begin, end));
    }
    
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

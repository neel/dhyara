/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_XQUEUE_H
#define DHYARA_XQUEUE_H

#include <iostream>
#include <cstdint>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "dhyara/frame.h"

namespace dhyara{

/**
 * creates a FreeRTOS queue
 */
template <typename MessageT, std::uint32_t Capacity>
class xqueue{
    QueueHandle_t _handle;
    
    public:
        /**
         * construct a queue
         */
        xqueue(){
            _handle = xQueueCreate(Capacity, sizeof(MessageT));
        }
        /**
         * enqueue a value to the queue
         */
        bool en(const MessageT& value, std::size_t ticks = 0){
            return pdPASS == xQueueSendToBack(_handle, (void*) &value, (TickType_t) ticks);
        }
        /**
         * Receive loop using a function as callback
         */
        template <typename F>
        void de(F fnc, std::size_t ticks = 0xffffffffUL){
            MessageT value;
            while (pdPASS == xQueueReceive(_handle, &value, (TickType_t) ticks)) {
                fnc(value);
            }
        }
        /**
         * Receive a message and return
         */
        MessageT de(std::size_t ticks = 0xffffffffUL){
            MessageT value;
            xQueueReceive(_handle, &value, (TickType_t) ticks);
            return value;
        }
        /**
         * Receives a message into the refered value from the queue. Returns success value
         * \code
         * MessageT msg;
         * while(xqueue_type::de(msg, ticks)){
         *      process(msg);
         * }
         * \endcode
         */
        bool de(MessageT& value, std::size_t ticks = 0xffffffffUL){
            return pdPASS == xQueueReceive(_handle, &value, (TickType_t) ticks);
        }
};

/**
 * Envelop for both <tt>to be sent</tt> and <tt>has been received</tt> messages
 */
struct message{
    inline message(): address(dhyara::peer_address::null()){}
    inline message(const dhyara::peer_address& a, const dhyara::frame& f): 
        address(a), frame(f) {}
    inline void clear() {
        address = dhyara::peer_address::null();
        frame.clear();
    }
    
    dhyara::peer_address    address;
    dhyara::frame           frame;
};

template <std::uint32_t Capacity>
struct network_fifo{
    typedef network_fifo<Capacity> self_type;
    typedef xqueue<message, Capacity> xqueue_type;
    typedef std::function<void (const dhyara::peer_address&, const dhyara::frame&)> callback_type;
    
    network_fifo(callback_type rcvf): _rcvf(rcvf){}
    void rcv_loop(std::size_t ticks = 0xffffffffUL){
        while(_rcv_queue.de(_rcvd_msg, ticks)){
            _rcvf(_rcvd_msg.address, _rcvd_msg.frame);
            _rcvd_msg.clear();
        }
    }

    /**
     * enqueue a frame that has been received
     */
    bool enqueue_received(const dhyara::peer_address& address, const dhyara::frame& frame, std::size_t ticks = 0){
        dhyara::message msg(address, frame);
        return _rcv_queue.en(msg, ticks);
    }
    private:
        xqueue_type   _rcv_queue;
        message       _rcvd_msg;
        callback_type _rcvf;
};


}

#endif // DHYARA_XQUEUE_H

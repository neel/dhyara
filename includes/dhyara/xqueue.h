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
 * Creates a FreeRTOS queue
 */
template <typename MessageT, std::uint32_t Capacity>
class xqueue{
    QueueHandle_t _handle;
    std::uint32_t _counter;
    
    public:
        /**
         * construct a queue
         */
        xqueue(): _counter(0){
            _handle = xQueueCreate(Capacity, sizeof(MessageT));
        }
        /**
         * enqueue a value to the queue
         * \param msg The message to be enqueued.
         * \param ticks The maximum amount of time the task should block waiting for space to become available on the queue, should it already be full. 
         */
        bool en(const MessageT& msg, std::size_t ticks = 0){
            bool success = (pdPASS == xQueueSendToBack(_handle, (void*) &msg, (TickType_t) ticks));
            if(success){
                ++_counter;
            }
            return success;
        }
        /**
         * enqueue a value to the queue, if the queue is full overwrites the last
         * \warning intended to be used on queues with single element
         * \param msg The message to be enqueued.
         */
        bool overwrite(const MessageT& msg){
            bool success = (pdPASS == xQueueOverwrite(_handle, (void*) &msg));
            if(success){
                _counter = 1;
            }
            return success;
        }
        /**
         * Receive loop using a function as callback
         * \param fnc The callback to be called with the received messages.
         * \param ticks The maximum amount of time the task should block waiting for an item to receive should the queue be empty at the time of the call.
         */
        template <typename F>
        void de(F fnc, std::size_t ticks = 0xffffffffUL){
            MessageT msg;
            while (pdPASS == xQueueReceive(_handle, &msg, (TickType_t) ticks)) {
                --_counter;
                fnc(msg);
            }
        }
        /**
         * Receive a message and return
         * \param ticks The maximum amount of time the task should block waiting for an item to receive should the queue be empty at the time of the call.
         */
        MessageT de(std::size_t ticks = 0xffffffffUL){
            MessageT msg;
            if(pdPASS == xQueueReceive(_handle, &msg, (TickType_t) ticks)){
                --_counter;
            }
            return msg;
        }
        /**
         * Receives a message into the refered value from the queue. Returns success value
         * \param msg Reference to message, where the received message will be copied to.
         * \param ticks The maximum amount of time the task should block waiting for an item to receive should the queue be empty at the time of the call.
         * \code
         * MessageT msg;
         * while(xq.de(msg, ticks)){
         *      process(msg);
         * }
         * \endcode
         */
        bool de(MessageT& msg, std::size_t ticks = 0xffffffffUL){
            bool success = (pdPASS == xQueueReceive(_handle, &msg, (TickType_t) ticks));
            if(success){
                --_counter;
            }
            return success;
        }
        /**
         * Peeks a message and return. `peek(0)` returns a message immediately if at least one exists, otherwise returns a default constructed empty message.
         * \param ticks The maximum amount of time the task should block waiting for an item to receive should the queue be empty at the time of the call.
         */
        MessageT peek(std::size_t ticks = 0xffffffffUL){
            MessageT msg;
            xQueuePeek(_handle, &msg, (TickType_t) ticks);
            return msg;
        }
        /**
         * Peeks a message into the refered value from the queue. Returns success value. `peek(0)` peeks a message immediately if at least one exists and returns true, otherwise returns false.
         * \param msg out parameter to put the message
         * \param ticks The maximum amount of time the task should block waiting for an item to receive should the queue be empty at the time of the call.
         * \code
         * MessageT msg;
         * while(xq.peek(msg, ticks)){
         *      process(msg);
         * }
         * \endcode
         */
        bool peek(MessageT& msg, std::size_t ticks = 0xffffffffUL){
            return pdPASS == xQueuePeek(_handle, &msg, (TickType_t) ticks);
        }
        
        /**
         * Return the number of messages stored in a queue.
         */
        UBaseType_t size(){
            return uxQueueMessagesWaiting(_handle);
        }
        
        /**
         * Returns the number of messages stored in the queue without making any FreeRTOS call.
         */
        std::uint32_t count() const{
            return _counter;
        }
        
        /**
         * Return the number of free spaces in a queue.
         */
        UBaseType_t free(){
            return uxQueueSpacesAvailable(_handle);
        }
        
        /**
         * Check whether the queue is empty or not.
         */
        bool empty() {
            return size() == 0;
        }
};

/**
 * watcher
 */
template <typename WatchedT>
struct watcher{
    typedef xqueue<WatchedT, 1> xqueue_type;
    
    xqueue_type _notification;
    
    /**
     * notify the watcher
     */
    bool notify(const WatchedT& watched){
        return _notification.overwrite(watched);
    }
    bool watch(WatchedT& watched){
        return _notification.de(watched, 0xffffffffUL);
    }
    bool sleeping(){
        return _notification.size() == 0;
    }
};

/**
 * notifier
 */
struct notifier: watcher<char>{
    bool notify(){
        return watcher<char>::notify(0);
    }
    /**
     * returns only when it is notified. sleeps otherwise. intended to be called from a while loop.
     */
    bool watch(){
        static char watched;
        return watcher<char>::watch(watched);
    }
};

/**
 * mutex
 */
struct mutex: watcher<char>{
    bool unlock(){
        return watcher<char>::notify(0);
    }
    /**
     * returns only when it is notified. sleeps otherwise. intended to be called from a while loop.
     */
    bool lock(){
        static char watched;
        return watcher<char>::watch(watched);
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
    inline bool valid() const {
        return !address.is_null();
    }
    
    dhyara::peer_address    address;
    dhyara::frame           frame;
};

template <std::uint32_t Capacity>
struct network_fifo{
    typedef network_fifo<Capacity> self_type;
    typedef xqueue<message, Capacity> xqueue_type;
    typedef std::function<void (const dhyara::peer_address&, const dhyara::frame&)> callback_type;
    
    /**
     * Construct with the callback
     */
    network_fifo(callback_type cb): _callback(cb){}
    /**
     * Start the receive loop that never returns
     */
    void loop(std::size_t ticks = 0xffffffffUL){
        while(_queue.de(_msg, ticks)){
            _callback(_msg.address, _msg.frame);
            _msg.clear();
        }
    }

    /**
     * enqueue a frame that has been received
     */
    bool enqueue(const dhyara::peer_address& address, const dhyara::frame& frame, std::size_t ticks = 0){
        dhyara::message msg(address, frame);
        return _queue.en(msg, ticks);
    }
    
    /**
     * Receives a frame into the refered value from the queue. Returns success value
     */
    bool dequeue(callback_type cb, std::size_t ticks = 0xffffffffUL){
        message m;
        bool success = _queue.de(m, ticks);
        cb(m.address, m.frame);
        return success;
    }
    /**
     * Peeks a frame into the refered value from the queue. Returns success value
     */
    bool peek(callback_type cb, std::size_t ticks = 0xffffffffUL){
        message m;
        bool success = _queue.peek(m, ticks);
        cb(m.address, m.frame);
        return success;
    }
    
    /**
     * Check whether the queue is empty or not.
     */
    bool empty() {
        return _queue.empty();
    }
    
    /**
     * Return the number of messages stored in a queue.
     */
    UBaseType_t size(){
        return _queue.size();
    }
    
    /**
     * Returns the number of messages stored in the queue without making any FreeRTOS call.
     */
    std::uint32_t count() const{
        return _queue.count();
    }
    private:
        xqueue_type   _queue;
        message       _msg;
        callback_type _callback;
};


}

#endif // DHYARA_XQUEUE_H

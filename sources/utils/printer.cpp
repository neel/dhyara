/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/utils/printer.h"
#include "esp_log.h"
#include <inttypes.h>

void dhyara::utils::printer::in(const dhyara::address& source, const dhyara::packets::beacon& beacon){
    ESP_LOGD(
        "dhyara", "<< BCN from %s at %" PRId64, 
        source.to_string().c_str(), 
        beacon.time()
    );
}

void dhyara::utils::printer::out(const dhyara::address& target, const dhyara::packets::beacon& beacon){
    ESP_LOGD(
        "dhyara", ">> BCN to %s at %" PRId64, 
        target.to_string().c_str(), 
        beacon.time()
    );
}

void dhyara::utils::printer::in(const dhyara::address& source, const dhyara::packets::acknowledgement& acknowledgement){
    ESP_LOGD(
        "dhyara", "<< ACK from %s of %" PRId64, 
        source.to_string().c_str(), 
        acknowledgement.time()
    );
}

void dhyara::utils::printer::out(const dhyara::address& target, const dhyara::packets::acknowledgement& acknowledgement){
    ESP_LOGD(
        "dhyara", ">> ACK to %s of %" PRId64, 
        target.to_string().c_str(), 
        acknowledgement.time()
    );
}

void dhyara::utils::printer::in(const dhyara::address& source, const dhyara::packets::advertisement& advertisement){
    ESP_LOGD(
        "dhyara", "<< ADV from %s dst %s delay %" PRId64, 
        source.to_string().c_str(), 
        advertisement.dest().to_string().c_str(), 
        advertisement.delay()
    );
}

void dhyara::utils::printer::out(const dhyara::address& target, const dhyara::packets::advertisement& advertisement){
    ESP_LOGD(
        "dhyara", ">> ADV to %s dst %s delay %" PRId64, 
        target.to_string().c_str(), 
        advertisement.dest().to_string().c_str(), 
        advertisement.delay()
    );
}

void dhyara::utils::printer::in(const dhyara::address& source, const dhyara::packets::echo_request& echo_request){
    ESP_LOGD(
        "dhyara", "<< ECHOQ from %s dst %s src %s seq %d ttl %d", 
        source.to_string().c_str(), 
        echo_request.target().to_string().c_str(), 
        echo_request.source().to_string().c_str(), 
        echo_request.seq(),
        echo_request.ttl()
    );
}

void dhyara::utils::printer::out(const dhyara::address& target, const dhyara::packets::echo_request& echo_request){
    ESP_LOGD(
        "dhyara", ">> ECHOQ to %s dst %s src %s seq %d ttl %d", 
        target.to_string().c_str(), 
        echo_request.target().to_string().c_str(), 
        echo_request.source().to_string().c_str(), 
        echo_request.seq(),
        echo_request.ttl()
    );
}

void dhyara::utils::printer::in(const dhyara::address& source, const dhyara::packets::echo_reply& echo_reply){
    ESP_LOGD(
        "dhyara", "<< ECHOR from %s dst %s src %s seq %d ttl %d delay %" PRId64, 
        source.to_string().c_str(), 
        echo_reply.target().to_string().c_str(), 
        echo_reply.source().to_string().c_str(), 
        echo_reply.seq(),
        echo_reply.ttl(),
        (esp_timer_get_time() - echo_reply.time())
    );
}

void dhyara::utils::printer::out(const dhyara::address& source, const dhyara::packets::echo_reply& echo_reply){
    ESP_LOGD(
        "dhyara", ">> ECHOR to %s dst %s src %s seq %d ttl %d", 
        source.to_string().c_str(), 
        echo_reply.target().to_string().c_str(), 
        echo_reply.source().to_string().c_str(), 
        echo_reply.seq(),
        echo_reply.ttl()
    );
}

void dhyara::utils::printer::in(const dhyara::address& source, const dhyara::packets::echo_lost& echo_lost){
    ESP_LOGD(
        "dhyara", "<< ECHOL from %s dst %s src %s seq %d delay %" PRId64, 
        source.to_string().c_str(), 
        echo_lost.target().to_string().c_str(), 
        echo_lost.source().to_string().c_str(), 
        echo_lost.seq(),
        (esp_timer_get_time() - echo_lost.time())
    );
}

void dhyara::utils::printer::out(const dhyara::address& source, const dhyara::packets::echo_lost& echo_lost){
    ESP_LOGD(
        "dhyara", ">> ECHOL to %s dst %s src %s seq %d", 
        source.to_string().c_str(), 
        echo_lost.target().to_string().c_str(), 
        echo_lost.source().to_string().c_str(), 
        echo_lost.seq()
    );
}

void dhyara::utils::printer::in(const dhyara::address& source, const dhyara::packets::chunk& chunk){
    ESP_LOGD(
        "dhyara", "<< CNK from %s for %s from %s packet %d pending %d", 
        source.to_string().c_str(), 
        chunk.target().to_string().c_str(), 
        chunk.source().to_string().c_str(), 
        (int)chunk.packet(), 
        (int)chunk.pending()
    );
}

void dhyara::utils::printer::out(const dhyara::address& target, const dhyara::packets::chunk& chunk){
    ESP_LOGD(
        "dhyara", ">> CNK to %s for %s from %s packet %d pending %d", 
        target.to_string().c_str(), 
        chunk.target().to_string().c_str(), 
        chunk.source().to_string().c_str(), 
        (int)chunk.packet(), 
        (int)chunk.pending()
    );
}

void dhyara::utils::printer::in(const dhyara::address& source, const dhyara::packets::delivered& delivered){
    ESP_LOGD(
        "dhyara", "<< DLV from %s for %s from %s packet %d", 
        source.to_string().c_str(), 
        delivered.target().to_string().c_str(), 
        delivered.source().to_string().c_str(), 
        (int)delivered.packet()
    );
}

void dhyara::utils::printer::out(const dhyara::address& target, const dhyara::packets::delivered& delivered){
    ESP_LOGD(
        "dhyara", ">> DLV to %s for %s from %s packet %d", 
        target.to_string().c_str(), 
        delivered.target().to_string().c_str(), 
        delivered.source().to_string().c_str(), 
        (int)delivered.packet()
    );
}

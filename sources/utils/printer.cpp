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

#include "dhyara/utils/printer.h"
#include "esp_log.h"

void dhyara::utils::printer::in(const dhyara::peer_address& source, const dhyara::packets::beacon& beacon){
    ESP_LOGI(
        "dhyara", "<< BCN from %s at %lld", 
        source.to_string().c_str(), 
        beacon.time()
    );
}

void dhyara::utils::printer::out(const dhyara::peer_address& target, const dhyara::packets::beacon& beacon){
    ESP_LOGI(
        "dhyara", ">> BCN to %s at %lld", 
        target.to_string().c_str(), 
        beacon.time()
    );
}

void dhyara::utils::printer::in(const dhyara::peer_address& source, const dhyara::packets::acknowledgement& acknowledgement){
    ESP_LOGI(
        "dhyara", "<< ACK from %s of %lld", 
        source.to_string().c_str(), 
        acknowledgement.time()
    );
}

void dhyara::utils::printer::out(const dhyara::peer_address& target, const dhyara::packets::acknowledgement& acknowledgement){
    ESP_LOGI(
        "dhyara", ">> ACK to %s of %lld", 
        target.to_string().c_str(), 
        acknowledgement.time()
    );
}

void dhyara::utils::printer::in(const dhyara::peer_address& source, const dhyara::packets::advertisement& advertisement){
    ESP_LOGI(
        "dhyara", "<< ADV from %s dst %s delay %lld", 
        source.to_string().c_str(), 
        advertisement.dest().to_string().c_str(), 
        advertisement.delay()
    );
}

void dhyara::utils::printer::out(const dhyara::peer_address& target, const dhyara::packets::advertisement& advertisement){
    ESP_LOGI(
        "dhyara", ">> ADV to %s dst %s delay %lld", 
        target.to_string().c_str(), 
        advertisement.dest().to_string().c_str(), 
        advertisement.delay()
    );
}

void dhyara::utils::printer::in(const dhyara::peer_address& source, const dhyara::packets::echo_request& echo_request){
    ESP_LOGI(
        "dhyara", "<< ICMPQ from %s dst %s src %s seq %d ttl %d", 
        source.to_string().c_str(), 
        echo_request.target().to_string().c_str(), 
        echo_request.source().to_string().c_str(), 
        echo_request.seq(),
        echo_request.ttl()
    );
}

void dhyara::utils::printer::out(const dhyara::peer_address& target, const dhyara::packets::echo_request& echo_request){
    ESP_LOGI(
        "dhyara", ">> ICMPQ to %s dst %s src %s seq %d ttl %d", 
        target.to_string().c_str(), 
        echo_request.target().to_string().c_str(), 
        echo_request.source().to_string().c_str(), 
        echo_request.seq(),
        echo_request.ttl()
    );
}

void dhyara::utils::printer::in(const dhyara::peer_address& source, const dhyara::packets::echo_reply& echo_reply){
    ESP_LOGI(
        "dhyara", "<< ICMPR from %s dst %s src %s seq %d ttl %d delay %lld", 
        source.to_string().c_str(), 
        echo_reply.target().to_string().c_str(), 
        echo_reply.source().to_string().c_str(), 
        echo_reply.seq(),
        echo_reply.ttl(),
        (esp_timer_get_time() - echo_reply.time())
    );
}

void dhyara::utils::printer::out(const dhyara::peer_address& source, const dhyara::packets::echo_reply& echo_reply){
    ESP_LOGI(
        "dhyara", ">> ICMPR to %s dst %s src %s seq %d ttl %d", 
        source.to_string().c_str(), 
        echo_reply.target().to_string().c_str(), 
        echo_reply.source().to_string().c_str(), 
        echo_reply.seq(),
        echo_reply.ttl()
    );
}

void dhyara::utils::printer::in(const dhyara::peer_address& source, const dhyara::packets::echo_lost& echo_lost){
    ESP_LOGI(
        "dhyara", "<< ICMPL from %s dst %s src %s seq %d delay %lld", 
        source.to_string().c_str(), 
        echo_lost.target().to_string().c_str(), 
        echo_lost.source().to_string().c_str(), 
        echo_lost.seq(),
        (esp_timer_get_time() - echo_lost.time())
    );
}

void dhyara::utils::printer::out(const dhyara::peer_address& source, const dhyara::packets::echo_lost& echo_lost){
    ESP_LOGI(
        "dhyara", ">> ICMPL to %s dst %s src %s seq %d", 
        source.to_string().c_str(), 
        echo_lost.target().to_string().c_str(), 
        echo_lost.source().to_string().c_str(), 
        echo_lost.seq()
    );
}

void dhyara::utils::printer::in(const dhyara::peer_address& source, const dhyara::packets::chunk& chunk){
    ESP_LOGI(
        "dhyara", "<< CNK from %s for %s from %s packet %d pending %d", 
        source.to_string().c_str(), 
        chunk.target().to_string().c_str(), 
        chunk.source().to_string().c_str(), 
        (int)chunk.packet(), 
        (int)chunk.pending()
    );
}

void dhyara::utils::printer::out(const dhyara::peer_address& target, const dhyara::packets::chunk& chunk){
    ESP_LOGI(
        "dhyara", ">> CNK to %s for %s from %s packet %d pending %d", 
        target.to_string().c_str(), 
        chunk.target().to_string().c_str(), 
        chunk.source().to_string().c_str(), 
        (int)chunk.packet(), 
        (int)chunk.pending()
    );
}

void dhyara::utils::printer::in(const dhyara::peer_address& source, const dhyara::packets::delivered& delivered){
    ESP_LOGI(
        "dhyara", "<< DLV from %s for %s from %s packet %d", 
        source.to_string().c_str(), 
        delivered.target().to_string().c_str(), 
        delivered.source().to_string().c_str(), 
        (int)delivered.packet()
    );
}

void dhyara::utils::printer::out(const dhyara::peer_address& target, const dhyara::packets::delivered& delivered){
    ESP_LOGI(
        "dhyara", ">> DLV to %s for %s from %s packet %d", 
        target.to_string().c_str(), 
        delivered.target().to_string().c_str(), 
        delivered.source().to_string().c_str(), 
        (int)delivered.packet()
    );
}

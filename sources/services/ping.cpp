/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/services/ping.h"

clipp::group dhyara::services::ping::options(){
    return (
        clipp::value("address", _target) % "target address",
        clipp::option("-b", "--batch") & clipp::value("size",    _batch) % "number of ICMPQ requests in one batch",
        clipp::option("-c", "--count") & clipp::value("count",   _count) % "number of batches to send",
        clipp::option("-w", "--wait")  & clipp::value("seconds", _wait)  % "number of seconds to wait after each batch of requests, for receiving all responses"
    );
}

esp_err_t dhyara::services::ping::run(dhyara::services::stream& stream){
    dhyara::address target(_target);
    if(target.valid()){
        services::ping_impl impl(stream, _count, _batch, _wait);
        impl.low_io(_low_io);
        impl(target);
        stream.finish();
    }
    return ESP_OK;
}

/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/services/ping.h"

dhyara::cmd::args<bool, std::uint32_t, std::uint32_t, std::uint32_t, std::string> dhyara::services::ping::options() {
    using namespace dhyara;
    return (
        cmd::pos("address", _target)                             % "target address",
        cmd::arg("b", "batch")  & cmd::value(_batch, "size")     % "number of ICMPQ requests in one batch",
        cmd::arg("c", "count")  & cmd::value(_count, "count")    % "number of batches to send",
        cmd::arg("w", "wait")   & cmd::value(_wait,  "seconds")  % "number of seconds to wait after each batch of requests, for receiving all responses",
        cmd::arg("q", "quiet")  & cmd::value(_quiet)             % "Don't show all replies, only return accumulated statistics"
    );
}

esp_err_t dhyara::services::ping::run(dhyara::services::stream& stream){
    dhyara::address target(_target);
    if(target.valid()){
        services::ping_impl impl(stream, _count, _batch, _wait);
        impl.quiet(_quiet);
        impl(target);
        stream.finish();
    }
    return ESP_OK;
}

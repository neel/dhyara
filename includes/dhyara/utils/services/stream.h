/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_SERVICES_STREAM_H
#define DHYARA_SERVICES_STREAM_H

#include <cstdint>
#include <esp_http_server.h>
#include <sstream>
#include <string>

namespace dhyara{
namespace utils{
namespace services{

struct stream{
    explicit stream(httpd_req_t* req);

    template <typename X>
    stream& write(const X& x){
        _sstream << x;
        write();
        return *this;
    }
    esp_err_t finish();
    inline std::uint64_t bytes() const { return _bytes; }
    private:
        void write();
    private:
        httpd_handle_t    _handle;
        int               _socket;
        std::uint64_t     _bytes;
        std::stringstream _sstream;
};



template <typename X>
inline stream& operator<<(stream& s, const X& x){
    return s.write(x);
}

}
}
}

#endif // DHYARA_SERVICES_STREAM_H
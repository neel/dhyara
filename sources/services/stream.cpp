#include "esp_err.h"
#include <ios>
#include <sstream>
#include <string>
#include <dhyara/services/stream.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <unistd.h>

static const char* crlf    = "\r\n";

dhyara::services::stream::stream(httpd_req_t* req): _bytes(0) {
    _socket = httpd_req_to_sockfd(req);
    _handle = req->handle;

    static const char* headers = "HTTP/1.1 200 OK\r\n"
                                 "Content-Type: text/plain\r\n"
                                 "Transfer-Encoding: chunked\r\n";

    send(_socket, headers, strlen(headers), 0);
    send(_socket, crlf,    strlen(crlf),    0);
}

void dhyara::services::stream::write(){
    int size = _sstream.rdbuf()->pubseekoff(0, std::ios_base::cur, std::ios_base::out);
    if(size > 0){
        std::string str(size, 0x0);
        _sstream.get(&str[0], size+1, 0x0);
        _sstream.clear();
        _sstream.rdbuf()->pubseekpos(0);

        _bytes += size;

        std::stringstream hexstream;
        hexstream << std::hex << str.size();

        std::string length = hexstream.str();

        send(_socket, length.c_str(), length.size(), 0);
        send(_socket, crlf,           strlen(crlf),  0);
        send(_socket, str.c_str(),    str.size(),    0);
        send(_socket, crlf,           strlen(crlf),  0);
    }
}


esp_err_t dhyara::services::stream::finish() {
    const char* zero = "0";
    send(_socket, zero, strlen(zero),  0);
    send(_socket, crlf, strlen(crlf),  0);
    send(_socket, crlf, strlen(crlf),  0);
    return ESP_OK;
}

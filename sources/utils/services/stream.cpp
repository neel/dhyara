#include <ios>
#include <sstream>
#include <string>
#include <dhyara/utils/services/stream.h>

#include <iostream>

dhyara::utils::services::stream::stream(httpd_req_t* req): _req(req), _bytes(0) {
    httpd_resp_set_type(_req, "text/plain");
}

void dhyara::utils::services::stream::write(){
    int size = _sstream.rdbuf()->pubseekoff(0, std::ios_base::cur, std::ios_base::out);
    if(size > 0){
        std::string str(size, 0x0);
        _sstream.get(&str[0], size+1, 0x0);
        _sstream.clear();
        _sstream.rdbuf()->pubseekpos(0);

        _bytes += size;

        httpd_resp_send_chunk(_req, str.c_str(), str.size());
    }
}


esp_err_t dhyara::utils::services::stream::finish(const char* err) {
    esp_err_t error = httpd_resp_set_status(_req, err);
    if(error != ESP_OK) 
        return error;
    return httpd_resp_sendstr_chunk(_req, 0x0);
}

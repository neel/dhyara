#include <sstream>
#include <string>
#include <dhyara/utils/services/stream.h>

dhyara::utils::services::stream::stream(httpd_req_t* req): _req(req), _bytes(0) {
    httpd_resp_set_type(_req, "text/plain");
}

void dhyara::utils::services::stream::write(std::stringstream& ss){
    ss.seekg(0, std::ios_base::end);
    int size = ss.tellg();

    std::string str;
    str.resize(size);
    ss.get(&str[0], size);

    _bytes += size;

    ESP_ERROR_CHECK(httpd_resp_send_chunk(_req, str.c_str(), size));
}


void dhyara::utils::services::stream::finish(const char* err) {
    httpd_resp_set_status(_req, err);
    ESP_ERROR_CHECK(httpd_resp_sendstr_chunk(_req, 0x0));
}

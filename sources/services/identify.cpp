/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/services/identify.h"
#include "dhyara/dhyara.h"
#include "esp_err.h"
#include <iomanip>

esp_err_t dhyara::services::identify::run(dhyara::services::stream &stream){
    dhyara::link& link = dhyara_link();

    wifi_config_t config;
    std::memset(&config, 0, sizeof(wifi_config_t));
    esp_err_t err = esp_wifi_get_config(WIFI_IF_AP, &config);
    std::string ssid;
    if(err == ESP_OK){
        std::string ssid_((const char*)config.ap.ssid, config.ap.ssid_len);
        ssid = ssid_;
    }

    stream << link.address().to_string() << " (" << ssid << ")" << "\n";

    stream.finish();
    return ESP_OK;
}
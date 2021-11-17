/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/services/phy_rate.h"
#include "dhyara/dhyara.h"
#include "esp_err.h"
#include <iomanip>
#include "esp_private/wifi.h"

dhyara::cmd::args<bool, bool, std::string> dhyara::services::phy_rate::options() {
    return (
        cmd::pos("rate", _phy_rate),
        cmd::arg("0", "disable") & cmd::value(_disable) % "Disable fix rate instead of enabling",
        cmd::arg("l", "list") & cmd::value(_list) % "List all available phy rates"
    );
}

esp_err_t dhyara::services::phy_rate::run(dhyara::services::stream &stream){
    wifi_phy_rate_t rate = WIFI_PHY_RATE_MAX;

    if(_phy_rate == "WIFI_PHY_RATE_1M_L"){
        rate = WIFI_PHY_RATE_1M_L;
    } else if(_phy_rate == "WIFI_PHY_RATE_2M_L"){
        rate = WIFI_PHY_RATE_2M_L;
    } else if(_phy_rate == "WIFI_PHY_RATE_5M_L"){
        rate = WIFI_PHY_RATE_5M_L;
    } else if(_phy_rate == "WIFI_PHY_RATE_11M_L"){
        rate = WIFI_PHY_RATE_11M_L;
    } else if(_phy_rate == "WIFI_PHY_RATE_2M_S"){
        rate = WIFI_PHY_RATE_2M_S;
    } else if(_phy_rate == "WIFI_PHY_RATE_5M_S"){
        rate = WIFI_PHY_RATE_5M_S;
    } else if(_phy_rate == "WIFI_PHY_RATE_11M_S"){
        rate = WIFI_PHY_RATE_11M_S;
    } else if(_phy_rate == "WIFI_PHY_RATE_48M"){
        rate = WIFI_PHY_RATE_48M;
    } else if(_phy_rate == "WIFI_PHY_RATE_24M"){
        rate = WIFI_PHY_RATE_24M;
    } else if(_phy_rate == "WIFI_PHY_RATE_12M"){
        rate = WIFI_PHY_RATE_12M;
    } else if(_phy_rate == "WIFI_PHY_RATE_6M"){
        rate = WIFI_PHY_RATE_6M;
    } else if(_phy_rate == "WIFI_PHY_RATE_54M"){
        rate = WIFI_PHY_RATE_54M;
    } else if(_phy_rate == "WIFI_PHY_RATE_36M"){
        rate = WIFI_PHY_RATE_36M;
    } else if(_phy_rate == "WIFI_PHY_RATE_18M"){
        rate = WIFI_PHY_RATE_18M;
    } else if(_phy_rate == "WIFI_PHY_RATE_9M"){
        rate = WIFI_PHY_RATE_9M;
    } else if(_phy_rate == "WIFI_PHY_RATE_MCS0_LG"){
        rate = WIFI_PHY_RATE_MCS0_LGI;
    } else if(_phy_rate == "WIFI_PHY_RATE_MCS1_LGI "){
        rate = WIFI_PHY_RATE_MCS1_LGI ;
    } else if(_phy_rate == "WIFI_PHY_RATE_MCS2_LGI"){
        rate = WIFI_PHY_RATE_MCS2_LGI;
    } else if(_phy_rate == "WIFI_PHY_RATE_MCS3_LGI"){
        rate = WIFI_PHY_RATE_MCS3_LGI;
    } else if(_phy_rate == "WIFI_PHY_RATE_MCS4_LGI "){
        rate = WIFI_PHY_RATE_MCS4_LGI ;
    } else if(_phy_rate == "WIFI_PHY_RATE_MCS5_LGI"){
        rate = WIFI_PHY_RATE_MCS5_LGI;
    } else if(_phy_rate == "WIFI_PHY_RATE_MCS6_LGI"){
        rate = WIFI_PHY_RATE_MCS6_LGI;
    } else if(_phy_rate == "WIFI_PHY_RATE_MCS7_LGI"){
        rate = WIFI_PHY_RATE_MCS7_LGI;
    } else if(_phy_rate == "WIFI_PHY_RATE_MCS0_SGI"){
        rate = WIFI_PHY_RATE_MCS0_SGI;
    } else if(_phy_rate == "WIFI_PHY_RATE_MCS1_SGI"){
        rate = WIFI_PHY_RATE_MCS1_SGI;
    } else if(_phy_rate == "WIFI_PHY_RATE_MCS2_SGI"){
        rate = WIFI_PHY_RATE_MCS2_SGI;
    } else if(_phy_rate == "WIFI_PHY_RATE_MCS3_SGI"){
        rate = WIFI_PHY_RATE_MCS3_SGI;
    } else if(_phy_rate == "WIFI_PHY_RATE_MCS4_SGI"){
        rate = WIFI_PHY_RATE_MCS4_SGI;
    } else if(_phy_rate == "WIFI_PHY_RATE_MCS5_SGI"){
        rate = WIFI_PHY_RATE_MCS5_SGI;
    } else if(_phy_rate == "WIFI_PHY_RATE_MCS6_SGI"){
        rate = WIFI_PHY_RATE_MCS6_SGI;
    } else if(_phy_rate == "WIFI_PHY_RATE_MCS7_SGI"){
        rate = WIFI_PHY_RATE_MCS7_SGI;
    } else if(_phy_rate == "WIFI_PHY_RATE_LORA_250K"){
        rate = WIFI_PHY_RATE_LORA_250K;
    } else if(_phy_rate == "WIFI_PHY_RATE_LORA_500K"){
        rate = WIFI_PHY_RATE_LORA_500K;
    } else{
        rate = WIFI_PHY_RATE_MAX;
    }

    if(!_disable && rate == WIFI_PHY_RATE_MAX){
        stream  << "Unknown phy rate " << _phy_rate  << "\n";
    }else if(rate != WIFI_PHY_RATE_MAX){
        esp_err_t err = esp_wifi_internal_set_fix_rate(WIFI_IF_AP, 1, rate);
        if(err != ESP_OK){
            stream << "Error encountered while enabling fixed phy rate" << _phy_rate << "\n" << esp_err_to_name(err) << "\n";
        }else{
            stream << "Done" "\n" "Note: the receiving nodes must be able to receive frames at this rate " "\n";
        }
        stream.finish();
        return ESP_OK;
    }else if(_disable){
        esp_err_t err = esp_wifi_internal_set_fix_rate(WIFI_IF_AP, 0, rate);
        if(err != ESP_OK){
            stream << "Error encountered while disabling fixed phy rate" << "\n" << esp_err_to_name(err) << "\n";
        }
        stream.finish();
        return ESP_OK;
    }

    if(_list || (!_disable && rate == WIFI_PHY_RATE_MAX)){
        stream  << "Available PHY rates" << "\n"
                <<  "\tWIFI_PHY_RATE_1M_L\n"
                    "\tWIFI_PHY_RATE_2M_L\n"
                    "\tWIFI_PHY_RATE_5M_L\n"
                    "\tWIFI_PHY_RATE_11M_L\n"
                    "\tWIFI_PHY_RATE_2M_S\n"
                    "\tWIFI_PHY_RATE_5M_S\n"
                    "\tWIFI_PHY_RATE_11M_S\n"
                    "\tWIFI_PHY_RATE_48M\n"
                    "\tWIFI_PHY_RATE_24M\n"
                    "\tWIFI_PHY_RATE_12M\n"
                    "\tWIFI_PHY_RATE_6M\n"
                    "\tWIFI_PHY_RATE_54M\n"
                    "\tWIFI_PHY_RATE_36M\n"
                    "\tWIFI_PHY_RATE_18M\n"
                    "\tWIFI_PHY_RATE_9M\n"
                    "\tWIFI_PHY_RATE_MCS0_LGI\n"
                    "\tWIFI_PHY_RATE_MCS1_LGI\n"
                    "\tWIFI_PHY_RATE_MCS2_LGI\n"
                    "\tWIFI_PHY_RATE_MCS3_LGI\n"
                    "\tWIFI_PHY_RATE_MCS4_LGI\n"
                    "\tWIFI_PHY_RATE_MCS5_LGI\n"
                    "\tWIFI_PHY_RATE_MCS6_LGI\n"
                    "\tWIFI_PHY_RATE_MCS7_LGI\n"
                    "\tWIFI_PHY_RATE_MCS0_SGI\n"
                    "\tWIFI_PHY_RATE_MCS1_SGI\n"
                    "\tWIFI_PHY_RATE_MCS2_SGI\n"
                    "\tWIFI_PHY_RATE_MCS3_SGI\n"
                    "\tWIFI_PHY_RATE_MCS4_SGI\n"
                    "\tWIFI_PHY_RATE_MCS5_SGI\n"
                    "\tWIFI_PHY_RATE_MCS6_SGI\n"
                    "\tWIFI_PHY_RATE_MCS7_SGI\n"
                    "\tWIFI_PHY_RATE_LORA_250K\n"
                    "\tWIFI_PHY_RATE_LORA_500K"
                << "\n";
    }

    stream.finish();
    return ESP_OK;
}
#ifndef MAINX_H
#define MAINX_H

#include "esp_now.h"

#ifdef __cplusplus
extern "C" {  
#endif  

void _sent(const uint8_t* target, esp_now_send_status_t status);
void _rcvd(const uint8_t* source, const uint8_t* data, int len);

void mainx();
  
#ifdef __cplusplus  
} // extern "C"  
#endif

#endif // MAINX_H

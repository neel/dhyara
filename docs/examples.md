Building Examples
==================

The examples are stored in the examples directory in the source.
To build an example (e.g. dhyara-example-basic)
```bash
cd examples/dhyara-example-basic
idf.py build
idf.py flash 
```

Structure 
----------

All examples puts the application code inside `application` class which has a `main()` method that runs an infinitite loop.
In `main.c` the `app_main()` function is defined which initializes NVS and WiFi. 
In the end it calls the `mainx()` function which is declared in `mainx.h` and defined in `mainx.cpp`.
In `mainx()` first ESP Now is initialized, then `application::main` is invoked.
Dhyara requires an instance of `dhyara::link` to maintain the ad hoc network.
It also requires the ESP Now send receive callbacks to be connected to the link.
So a global instance of `dhyara::link` is created.

```cpp

dhyara::link g_link;

void _sent(const uint8_t* target, esp_now_send_status_t status){
    g_link._esp_sent_cb(target, status);
}

void _rcvd(const uint8_t* source, const uint8_t* data, int len){
    g_link._esp_rcvd_cb(source, data, len);
}

```
The two global functions `_sent` and `_rcvd` are used as send and receive callbacks that forward the arguments to the `link` instance. 

```cpp

void mainx(){
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(_sent));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(_rcvd));
    
    dhyara::network network(g_link);
    network.start();
    
    esp_log_level_set("dhyara", ESP_LOG_VERBOSE);
    
    application app(network);
    app.main();
} 
```
In the `mainx()` first a network object is created.
The `dhyara::network::start` method does not block. 
It creates the necessary FreeRTOS tasks to initiate and maintain the network and returns.
Then the `application` class is instantiated using a reference to that network, and then its `main()` method is called.
The application code can be found in the `application::main` method defined in `application.cpp`.

Building Examples {#examples}
==================

The examples are stored in the examples directory in the source.
To build an example (e.g. hello-cxx)

```bash
cd examples/hello-cxx
idf.py build
idf.py flash 
```

The `hello-c` and `hello-cxx` examples are respectively C an C++ applications using dhyara. 
Botyh of these applications assumes there is a source and a sink node.

In the C example (hello-c) the source and sink are defined as 6 byte arrays
~~~{.c}
uint8_t source[] = {0x4c, 0x11, 0xae, 0x71, 0x0f, 0x4d};
uint8_t sink[]   = {0x4c, 0x11, 0xae, 0x9c, 0xa6, 0x85};
~~~
In the C++ example the `dhyara::peer_address` class is used.
~~~{.cpp}
dhyara::peer_address sink("4c:11:ae:9c:a6:85"), source("4c:11:ae:71:0f:4d");
~~~

While running this examples, usercode *MUST* change the mac address and replace it with the mac address of the ESP32 node.
Both of these examples are proggrammed in a way so that the source and the sink repeatedly pings each other and performs traceroute operation.

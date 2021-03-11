DHYARA
======

Dhyara is a ESP-IDF component for Mobile Ad-Hoc Networks on the top of ESP NOW. Dhyara is Open Source under BSD 2 Clause License. See [LICENSE](docs/LICENSE.md)

Compilation & Integration
-----------

Dhyara is a C++11 library that has to be used as an ESP-IDF component. 
It can be cloned to the `components` directory or can be used as a `git submodule` inside your projects `components` directory. 
If the application project is using an extra components directory then path to that directory has to be added in the root directory's CMakeLists.txt file as shown in the following example.

```
set(EXTRA_COMPONENT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/components)
```

To configure the network parameters use `idf.py menuconfig` in the aplications directory and Go to *Component config > Dhyara ad hoc network*
To build and flash the application use `idf.py build` and `idf.py flash` as usual.

See [Building the examples](docs/examples.md).

Guide
------

ESP wifi subsystem has to be started before starting the dhyara network.
`dhyara::link` provides the underlying communication layer over ESP NOW. 
The registered received callbacks should forward the received data to the `dhyara::link::_esp_rcvd_cb`.
Similarly the registered sent callback should forward the arguments to the `dhyara::link::_esp_sent_cb`.
A `dhyara::network` instance is created by using a reference to the `link`.
The network is started using nonblocking `dhyara::network::start` method, which creates multiple tasks to maintain the underlying network.
For rest of the application using dhyara, a reference to the `dhyara::network` is required.
So after starting the network, the application code should pass a reference to the network instance appopriately.
All nodes running dhyara will communicate with each other and transmit various packets to maintain the dynamic neighbourhood.

### Design

Dhyara uses many different packets for communication. 
The packets are defined in the \ref packets module. 
Type of a packet is identified using `dhyara::packets::type` enum.
Once a packet is received it is read and enqueued to a queue for further processing. 
Different actions have been defined to process different packets. 
The actions are defined in the \ref actions module.
The actions are installed to the `dhyara::link` by passing the packet type and a reference to the action to `dhyara::link::operator[]()` overload.
While dequeuing a packet it finds the appropriate action through the packet type and invokes the action.
The action may need to send some other packet in response.
For such circumstances the action may take a reference to the `dhyara::link` in its constructor.
Depending of the type of packet received, and the action installed, the received packet may be enqueued into another queue for further processing.
Each action provides a \ref dhyara::actions::action<DerivedT, PacketT>::connect "connect" method to bind a callback which will be called after the action have processed the incomming packet.
The `dhyara::network` provides functions to get reference to all installed actions.

### Usage

Being a network over L2 all nodes are identified through their MAC address. `dhyara::peer_address` abstracts a MAC address representation. 
ESP Now uses Wifi Action Frames to communicate, which limits maximuum size of a single packet to 250 bytes. 
So if the application want to send variable sized messages then dhyara may split that message into multiple chunks and send those packets to the desired destination through the best route over the ad hoc network. 
To send a message, use `dhyara::network::send`, which takes a destination address and `begin` and `end` iterators to the data that has to be sent.

Protocol
---------

Dhyara does not depend in IP layer. It uses MAC address to identify the nodes in the network. 
To establish an ad hoc network each nodes broadcasts beacon periodically which is acknowledged by its neighbourhood.
The multihop network is established by sharing the routes discoverd by each of these nodes. See [Protocol](docs/protocol.md) for details.

Additionally ICMP is implemented on the top of L2 to have a ping and traceroute like functionality.

Example
--------

The example below assumes that the application coed is written in a class named `application` which takes a reference to `dhyara::network` in its constructor and stores it in a member variable `_network`.
It is also assumed that the `main` method of the `application` class is called through the main.

```cpp
void application::main(){
    // defining a source and a target in the multi hop network
    dhyara::peer_address target("4c:11:ae:9c:a6:85"), source("4c:11:ae:71:0f:4d");
    
    // The anonymous function will be called once all chunks of a data packet is received
    auto lambda = [](const dhyara::peer::address&, const dhyara::packets::data& data){
        std::cout << "data received from " << data.source() << std::endl;
        std::string buffer;
        data.copy(std::back_inserter(buffer));
        std::cout << buffer << std::endl;
    };
    
    // Bind the lambda to be called after a data is received
    _network.on_data(lambda);
    
    while(1){
        // Assuming the same application is running on all nodes
        // The code below should only run in the source node
        if(_network.link().address() == source){
            // A data to be sent (strign used in the example, but can be anything that can be iterated as a sequence of bytes)
            std::string buffer = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.";
            
            // send the dynamically sized data
            _network.send(target, buffer.begin(), buffer.end());
        }
        
        // Sleep for 5s
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
```
See [Building the examples](docs/examples.md).



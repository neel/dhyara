DHYARA
======

Overview
-----------

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

Guide
-----------

### Initialization

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


### Example

The example below assumes that the application coed is written in a class named `application` which takes a reference to `dhyara::network` in its constructor and stores it in a member variable `_network`.
It is also assumed that the `main` method of the `application` class is called through the main.

```cpp
void application::main(){
    // Use ping tool to get connection statistics
    dhyara::tools::ping ping(_network);
    // Configure the ping tool
    ping.count(1).batch(1).sleep(1);
    
    // defining a source and a target in the multi hop network
    dhyara::peer_address target("4c:11:ae:9c:a6:85"), source("4c:11:ae:71:0f:4d");
    
    // For experimental purpose
    // Optionally banning direct communication between the source and the target
    // Forcing the source and target to communicate via one or more intermediate nodes
    if(_network.link().address() == target) _network.beacon().ban(source);
    if(_network.link().address() == source) _network.beacon().ban(target);
    
    // The anonymous function will be called once all chunks of a data packet is received
    auto lambda = [](const dhyara::peer::address& source, const dhyara::packets::data& data){
        std::cout << "data received from " << source << std::endl;
        std::string buffer;
        data.copy(std::back_inserter(buffer));
        std::cout << buffer << std::endl;
    };
    
    // Bind the lambda to be called Once a data is received
    _network.chunk().on_data(lambda);
    
    while(1){
        // Assuming the same application is running on all nodes
        // The code below should only run in the source node
        if(_network.link().address() == source){
            // Ping the target (through multi hop network)
            ping(target);
            
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


Protocol
-----------

### Routing Table

Each node `Xᵢ ∊ X` maintains a **routing table** of the following form which is gradually built up by the protocol.  Each row of this table denotes a route to a destination node (first column) via an intermediate nde (second column) and its corresponding delay. 0x0 is used to depict direct communication (not via any intermediate node)

```
Xⱼ 0x0 𝛿ⱼ₀ ∀ j ≠ i
Xⱼ Xₖ  𝛿ⱼₖ ∀ j,k ≠ i and ∀ j ≠ k
```

Based on this table a **best vector** is calculated 

```
Xⱼ ← Min(y ∊ Intermediate(Xⱼ)) ∀ j ≠ i
```

While sending data to `Xⱼ` the best vector is looked for the immediate neighbor 

### Beacon (BCN)

Every node `x ∊ X` broadcasts a beacon packet at a fixed interval to `0xff:0xff:0xff:0xff:0xff:0xff`. The packet contains `Tx` (time at node `x`).

### Acknowledgement (ACK)

A node `y ∊ X` when receives the broadcasted beacon, adds `x` to its peers and sends an acknowledgement packet to `x` with the same `Tx` of the beacon packet.

The node `x` on receiving the acknowledgement calculates RTT as `(T - Tx)` where T is the current time on node `x`. The delay of one trip is calculated as `𝛿 = RTT / 2`

The node `x` adds / updates `(x, y, 𝛿)` in its routing table and follows the `sync_neighbors` routine.

#### sync_neighbors

If any operation on the routing table of `Xᵢ` alters the best route or the delay of the best route for destination `Xⱼ` then an `Advertisement` packet is sent to `∀ Xₖ ∊ X : k ≠ j`

The Advertisement packet contains the destination node `Xⱼ` and `𝛿ⱼᵢ` the minimum delay to reach that node from `Xᵢ`. Node `Xᵢ` does not advertise the best intermediate node (which can be 0x0 also).

If the best vector is not altered by routing table update then no `advertisement` packet is sent

However if `∃ Xₖ ∊ X` for which no broadcast packet has been sent for a large amount of time then an advertisement packet is sent even if there is not change in the best vector.

#### depreciation

If a node `Xᵢ` doesn't receive any beacon from node `Xⱼ` for a threshold, the delay of all routes involved with `Xᵢ` is doubled. Then thr routing table update follows  the `sync_neighbors` routine.

Once the delay reaches the integer boundary then the node is remove from thr routing table after transmiting the advertisement packet in the `sync_neighbours` routine.

### Advertisement (ADV)

An Advertisement packet originated from `Xᵢ` contains a destination mac `Xⱼ` and delay `d`.

`Xₜ` updates route `(Xⱼ Xᵢ d + 𝛿ⱼᵢ)` in its routing table and follows the `sync_neighbors` routine. The value of is obtained from the existing routes in the routing table.

### Data & Chunk (CNK)

### Delivery Report (DLV)



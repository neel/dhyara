DHYARA
======

Dhyara is a DVR based implementation for Mobile Ad-Hoc Networks on the top of ESP NOW primarily targetted for ESP32.

Compilation
-----------

```
git clone https://gitlab.com/neel.basu/dhyara.git
cd dhyara
git checkout develop
idf.py build flash monitor
```

### Usage

To queue a string (signed char buffer) to be sent

```cpp
std::string msg = "A string of any size";
medium.enqueue("ac:67:b2:0b:b8:65", msg); // ac:67:b2:0b:b8:65 is the final destination
```

To send a data packet

```cpp
std::array<std::uint8_t, 2048> buff;
dhyara::packets::data packet(dhyara::peer::address("ac:67:b2:0b:b8:65"), buff.begin(), buff.end()); // ac:67:b2:0b:b8:65 is the final destination
medium.enqueue(packet); // packet is queued for transmission
medium.send(dhyara::peer::address("ac:67:b2:0b:b8:65"), packet); // To send the packet immediately
```

The `buff` may be a string, array, vector or any user implemented forward iterable container. The iterator is dereferenced and the value is copied as `uint8_t` into the data packet.

```cpp
SomePlainOldDataType value;
const std::uint8_t* buff = reinterpret_cast<const std::uint8_t*>(&value);
dhyara::packets::data packet(dhyara::peer::address("ac:67:b2:0b:b8:65"), buff, buff+sizeof(SomePlainOldDataType));
```

`medium` will transmit the data packet through the multi hop network to the final destination. `medium` may also split the data into multiple chunk if necessary. If splitted these chunks will be joined in the final destination.

Protocol
--------

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



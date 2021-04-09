Protocol {#protocol}
=========

Dhyara Commnunication Protocol

Routing Table
--------------

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

### Neighbourhood Synchronization ###

If any operation on the routing table of `Xᵢ` alters the best route or the delay of the best route for destination `Xⱼ` then an `Advertisement` packet is sent to `∀ Xₖ ∊ X : k ≠ j`

The Advertisement packet contains the destination node `Xⱼ` and `𝛿ⱼᵢ` the minimum delay to reach that node from `Xᵢ`. Node `Xᵢ` does not advertise the best intermediate node (which can be 0x0 also).

If the best vector is not altered by routing table update then no `advertisement` packet is sent

However if `∃ Xₖ ∊ X` for which no broadcast packet has been sent for a large amount of time then an advertisement packet is sent even if there is not change in the best vector.

### Route Depreciation ###

If a node `Xᵢ` doesn't receive any beacon from node `Xⱼ` for a threshold, the delay of all routes involved with `Xᵢ` is doubled. Then thr routing table update follows  the `sync_neighbors` routine.

Once the delay reaches the integer boundary then the node is remove from thr routing table after transmiting the advertisement packet in the `sync_neighbours` routine.

Packets
--------

## Basic ##

### Beacon (BCN) ###

Every node `x ∊ X` broadcasts a beacon packet at a fixed interval to `0xff:0xff:0xff:0xff:0xff:0xff`. The packet contains `Tx` (time at node `x`).

### Acknowledgement (ACK) ###

A node `y ∊ X` when receives the broadcasted beacon, adds `x` to its peers and sends an acknowledgement packet to `x` with the same `Tx` of the beacon packet.

The node `x` on receiving the acknowledgement calculates RTT as `(T - Tx)` where T is the current time on node `x`. The delay of one trip is calculated as `𝛿 = RTT / 2`

The node `x` adds / updates `(x, y, 𝛿)` in its routing table and follows the `sync_neighbors` routine.


### Advertisement (ADV) ###

An Advertisement packet originated from `Xᵢ` contains a destination mac `Xⱼ` and delay `d`.

`Xₜ` updates route `(Xⱼ Xᵢ d + 𝛿ⱼᵢ)` in its routing table and follows the `sync_neighbors` routine. The value of is obtained from the existing routes in the routing table.

### Data ###

#### Chunk (CNK) ####

#### Delivery Report (DLV) ####

## Auxilary ##

### ICMP over L2 ###

#### ICMP Request (ICMPQ) ####

#### ICMP Reply (ICMPR) ####

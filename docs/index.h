/**
 * \defgroup dhyara Dhyara
 * \brief Dhyara Communication Interface provides a simplified abstraction over the underlying Ad-Hoc Network. 
 * Unless there is a need to customize the internal mechanism of maintaining the usercode only needs to interact through this module.
 * Dhyara is written in C++11. Some functionalities are exposed as C functions, so that it can also be incorporated into a C project.
 * 
 * ### Architecture ###
 * \ref dhyara::link is responsible for performing the low level IO operations in the network. 
 * \ref dhyara::link also maintains the neighbourhood, routing and message queue for sending and receiving frames over the network.
 * \ref dhyara::network configures the link and creates FreeRTOS tasks to initialize and maintain the Ad-Hoc Network.
 * \ref dhyara::network also provides convenience functions to interact with the underlying link. 
 * There should be exactly one instance of the link and the network.
 * While initializing dhyara, certain functions \ref dhyara::link are required to be plugged in as callbacks to the espnow and wifi subsystem.
 * 
 * ### Initialization ###
 * Dhyara runs on the top of ESP Now. So it requires wifi and espnow to be initialized and started in order to work.
 * A \ref dhyara::link is constructed, which can be accessed by \ref dhyara_link.
 * The functions \ref dhyara_init and \ref dhyara_start_default_network is used to initialize and start the network.
 * \code
 * ESP_ERROR_CHECK(dhyara_init(WIFI_MODE_AP));
 * dhyara_start_default_network();
 * \endcode
 * These two functions perform the necessary initialization steps for wifi, espnow and dhyara.
 * While calling these two functions, the usercode may directly interact with the wifi subsystem with the ESP-IDF APIs in order to configure the network interfaces.
 * \ref dhyara_init performs three operations. 
 * 
 * * \ref dhyara_wifi_init to initialize wifi (e.g. Initializing and creating default network interfaces for AP and STA and registering event handlers etc.)
 * * \ref esp_wifi_start to start the wifi
 * * \ref dhyara_espnow_init to initialize espnow and register the \ref dhyara::link functions as espnow callbacks 
 * 
 * The network is an instance of \ref dhyara::network which is maintained as a global pointer (set to null by default).
 * \ref dhyara_start_default_network creates a network by using \ref dhyara_create_default_network, if it is not already set. 
 * Usercode may use \ref dhyara_set_default_network to set an user created network as default.
 * 
 * \attention Usercode may completely bypass the creation of a global network and maintain the network in the usercode. 
 * A few (non-essential) functions like \ref dhyara_ping, \ref dhyara_traceroute, \ref dhyara_send, \ref dhyara_receive, \ref dhyara_receivef won't work if a global network is not set.
 * These above functions are supposesed to be C compatiable, hence can not take the \ref dhyara::network as an argument.
 * However all these functionalities are doable with equivalent C++ API that do not require a global \ref dhyara::link.
 * 
 * \ref dhyara_start_default_network also starts the network using \ref dhyara::network::start, which creates the necessary FreeRTOS tasks to maintain the network.
 * 
 * ### Basic Operations ###
 * Basic send/receive IO is performed using variable length \ref dhyara::packets::data packets. 
 * A data packet is splitted into smaller sized chunks (\ref dhyara::packets::chunk), which are aggregated on the receiving side.
 * On the receiving side the data packet is received from an intermediate node which may not be the node from where the data has originated (because the data is delivered through a multihop network).
 * \ref dhyara_receive, \ref dhyara_receivef performs the basic send/receive operations using the global instance of \ref dhyara::network.
 * 
 */

/**
 * \defgroup routing Routing 
 * \brief Dhyara Routing module is responsible for maintaining the distributed routing table across the network.
 * Each node `Xᵢ ∊ X` maintains a **routing table** of the following form which is gradually built up by the protocol.  
 * Each row of this table denotes a route to a destination node (first column) via an intermediate nde (second column) and its corresponding delay. 
 * 0x0 is used to depict direct communication (not via any intermediate node).
 * 
 * ```
 * Xⱼ 0x0 𝛿ⱼ₀ ∀ j ≠ i
 * Xⱼ Xₖ  𝛿ⱼₖ ∀ j,k ≠ i and ∀ j ≠ k
 * ```
 * 
 * Based on this table a **best vector** is calculated 
 * 
 * ```
 * Xⱼ ← Min(y ∊ Intermediate(Xⱼ)) ∀ j ≠ i
 * ```
 * 
 * While sending data to `Xⱼ` the best vector is looked for the immediate neighbor 
 * 
 * ### Neighbourhood Synchronization ###
 * 
 * If any operation on the routing table of `Xᵢ` alters the best route or the delay of the best route for destination `Xⱼ` then an `Advertisement` packet is sent to `∀ Xₖ ∊ X : k ≠ j`
 * The Advertisement packet contains the destination node `Xⱼ` and `𝛿ⱼᵢ` the minimum delay to reach that node from `Xᵢ`. Node `Xᵢ` does not advertise the best intermediate node (which can be 0x0 also).
 * If the best vector is not altered by routing table update then no `advertisement` packet is sent
 * However if `∃ Xₖ ∊ X` for which no broadcast packet has been sent for a large amount of time then an advertisement packet is sent even if there is not change in the best vector.
 * 
 * ### Route Depreciation ###
 * 
 * If a node `Xᵢ` doesn't receive any beacon from node `Xⱼ` for a threshold, the delay of all routes involved with `Xᵢ` is doubled. Then thr routing table update follows  the `sync_neighbors` routine.
 * Once the delay reaches the integer boundary then the node is remove from thr routing table after transmiting the advertisement packet in the `sync_neighbours` routine.
 */

/**
 * \defgroup packets Packets 
 * \brief Dhyara Packets provide definitions for all packets that are sent/received over the network.
 * 
 * ## Basic ##
 * 
 * The following packets are used for maintaining the basic network.
 * 
 * ### Beacon (BCN) ###
 * 
 * Every node `x ∊ X` broadcasts a beacon packet at a fixed interval to `0xff:0xff:0xff:0xff:0xff:0xff`. The packet contains `Tx` (time at node `x`).
 * 
 * ### Acknowledgement (ACK) ###
 * 
 * A node `y ∊ X` when receives the broadcasted beacon, adds `x` to its peers and sends an acknowledgement packet to `x` with the same `Tx` of the beacon packet.
 * The node `x` on receiving the acknowledgement calculates RTT as `(T - Tx)` where T is the current time on node `x`. The delay of one trip is calculated as `𝛿 = RTT / 2`
 * The node `x` adds / updates `(x, y, 𝛿)` in its routing table and follows the `sync_neighbors` routine.
 * 
 * ### Advertisement (ADV) ###
 * 
 * An Advertisement packet originated from `Xᵢ` contains a destination mac `Xⱼ` and delay `d`.
 * `Xₜ` updates route `(Xⱼ Xᵢ d + 𝛿ⱼᵢ)` in its routing table and follows the `sync_neighbors` routine. The value of is obtained from the existing routes in the routing table.
 * 
 * ### Data ###
 * 
 * ESP Now limits maximum size of payload in each frame to 250 bytes only. So variable length data like \ref dhyara::packets::data is splitted into multiple chunks.
 * 
 * #### Chunk (CNK) ####
 * #### Delivery Report (DLV) ####
 * 
 * ## Auxilary ##
 * ### ICMP over L2 ###
 * #### ICMP Request (ICMPQ) ####
 * #### ICMP Reply (ICMPR) ####
 * 
 * \{
 */

/**
 * \defgroup io IO
 * \ingroup packets 
 * \brief Serialization/Deserialization and Input/Output of the packets are performed in this module.
 */

/**
 * \}
 */

/**
 * \defgroup tools Tools
 * \brief Dhyara Tools.
 */


/**
 * \defgroup actions Actions
 * \brief Dhyara actions.
 */




/**
 * \defgroup dhyara Dhyara
 * \brief Dhyara Communication Interface provides a simplified abstraction over the underlying Ad-Hoc Network. 
 * 
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
 * A \ref dhyara::link is constructed, which can be accessed by `dhyara_link`.
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
 * * `esp_wifi_start()` to start the wifi
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
 * 
 * ### Routing Table ###
 * 
 * Each node \f$X_{i} \in X\f$ maintains a Routing table (\f$ R_{i} \f$) of the following form which is gradually built up by the protocol.  
 * Each row of this table denotes a route to a destination node (first column) via an intermediate nde (second column) and its corresponding delay. 
 * The protocol ensures that \f$\forall X_{i}\in X\f$ is aware about existance of all other nodes in \f$X \setminus X_{i}\f$. 
 * For all neighbours \f$X_{j} \in Neibourhood(X_{i})\f$ \f$\exists u \in R_{i}\;s.t.Intermediate(u) = 0\f$
 * For all non-neighbours \f$X_{k} \not\in Neibourhood(X_{i})\f$ \f$\exists u \in R_{i}\;s.t.Intermediate(u) \in Neibourhood(X_{i})\f$
 * The routing table may contain multiple routes for the same distination (with different intermediate nodes)
 * 
 * \f[
 *      R_{i} = \begin{cases}
 *          (X_{j}, 0, \delta_{j,0}) & \forall X_{j} \in Neibourhood(X_{i}) \\
 *          (X_{k}, X_{j}, \delta_{j,0}) & \forall X_{k} \not\in Neibourhood(X_{i}) \;s.t.\; X_{j} \in Neibourhood(X_{i})
 *      \end{cases}
 * \f]
 * 
 * - 0 is used as an intermediate node to depict direct communication (not via any intermediate node).
 * - Delay \f$\delta_{x,y}\f$ is the expected delay of sending to \f$X_x\f$ via \f$X_y\f$ from \f$X_{i}\f$
 * 
 * Based on the above mentioned table a best route \f$Best(X_{t}, i) \in R_{i}\f$ is calculated in \f$X_{i}\f$ for each destination \f$t\f$ (w.r.t. source \f$X_{i}\f$)
 * 
 * \f[
 *     Best(X_{t}, i) = u \;s.t.\; Delay(u) \leq Delay(v)\; \forall\,v\,\in\,R_{i}
 * \f]
 * 
 * While sending data to \f$X_{t}\f$ the best route is looked for the intermediate node.
 * 
 * ### Neighbourhood Synchronization ###
 * 
 * Each node \f$X_{i}\f$ broadcasts a \ref dhyara::packets::beacon "Beacon" at a fixed interval.
 * The \ref dhyara::packets::beacon "Beacon" contains Time at \f$X_{i}\f$ (\f$T_{i}\f$) at the time of sending.
 * The \ref dhyara::packets::beacon "Beacon" is received by all \f$X_{j}\f$ in the neighbourhood.
 * The neighbours \f$\forall X_{j} \in Neibourhood(X_{i})\f$ respond with an \ref dhyara::packets::acknowledgement "Acknowledgement", which contains the same \f$T_{i}\f$ which was received in the \ref dhyara::packets::beacon "Beacon".
 * \f$X_{i}\f$ after receiving the \ref dhyara::packets::acknowledgement "Acknowledgement" from \f$X_{j}\f$ calculates \f$\delta_{j,0}\f$ as \f$T_{i}/2\f$ and adds that as a route in \f$R_{i}\f$.
 * 
 * If any operation on the routing table \f$R_{i}\f$ alters/creates a best route or the delay of a best route for a destination \f$X_{t}\f$ then an \ref dhyara::packets::advertisement "Advertisement" is sent to \f$X_{j}\,\forall X_{j} \in Neibourhood(X_{i})\f$ from \f$X_{i}\f$.
 * The \ref dhyara::packets::advertisement "Advertisement" contains the destination node \f$X_{t}\f$ and \f$\delta_{t,i}\f$ the minimum delay to reach \f$X_{t}\f$ from \f$X_{i}\f$. 
 * The best intermediate node (which may be 0 also) is not advertised.
 * If the best vector is not altered by the updatation of routing table then no Advertisement packet is sent.
 * However if \f$\exists X_{k} ∊ X\f$ for which no broadcast packet has been sent for a large amount of time then an \ref dhyara::packets::advertisement "Advertisement" regarding \f$X_{k}\f$ is sent even if there is not change in the best vector.
 * 
 * ### Route Depreciation ###
 * 
 * If a node \f$X_{i}\f$ doesn't receive any beacon from node \f$X_{j}\f$ for a fixed amount of time, the delay of all routes involved with \f$X_{j}\f$ is multiplied with a factor. 
 * Then the routing table update follows the above mentioned Neighbourhood Synchronization.
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
 * ### ECHO over L2 ###
 * #### ECHO Request (ECHOQ) ####
 * #### ECHO Reply (ECHOR) ####
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




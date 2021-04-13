/**
 * \defgroup dhyara Dhyara
 * Dhyara Communication Interface provides a simplified abstraction over the underlying Ad-Hoc Network. 
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
 * Dhyara Routing
 */

/**
 * \defgroup packets Packets 
 * Dhyara Packets
 * \{
 */

/**
 * \defgroup io IO
 * \ingroup packets 
 * Packets Input Output
 */

/**
 * \}
 */

/**
 * \defgroup tools Tools
 * Dhyara Tools
 */


/**
 * \defgroup actions Actions
 * Dhyara actions
 */




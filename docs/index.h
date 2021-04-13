/**
 * \defgroup dhyara Dhyara
 * Dhyara Communication Interface provides a simplified abstraction over the underlying Ad-Hoc Network. 
 * Unless there is a need to customize the internal mechanism of maintaining the usercode only needs to interact through this module.
 * Dhyara is written in C++11. Some functionalities are exposed as C functions, so that it can also be incorporated into a C project.
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
 * In the examples \ref dhyara_init and \ref dhyara_start_default_network is used to initialize and start dhyara network.
 * \code
 * ESP_ERROR_CHECK(dhyara_init(WIFI_MODE_AP));
 * dhyara_start_default_network();
 * \endcode
 * These two functions perform the necessary initialization steps for wifi, espnow and dhyara.
 * While calling these two functions, the usercode may directly interact with the wifi subsystem with the ESP-IDF APIs in order to configure the network interfaces.
 * `dhyara_init` performs three operations. 
 * 
 * * \ref dhyara_wifi_init to initialize wifi
 * * \ref esp_wifi_start to start the wifi
 * * \ref dhyara_espnow_init to initialize espnow
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




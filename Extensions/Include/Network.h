/*
 * Network.h
 *
 *  Created on: 22 апр. 2022 г.
 *      Author: producer
 */

#ifndef COMPONENTS_ESP32_SYSTEM_EXTENSIONS_INCLUDE_NETWORK_H_
#define COMPONENTS_ESP32_SYSTEM_EXTENSIONS_INCLUDE_NETWORK_H_

#include "ESP32_System.h"



// check includes
#include <string.h>
//#include "sdkconfig.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#if CONFIG_EXAMPLE_CONNECT_ETHERNET
#include "esp_eth.h"
#if CONFIG_ETH_USE_SPI_ETHERNET
#include "driver/spi_master.h"
#endif // CONFIG_ETH_USE_SPI_ETHERNET
#endif // CONFIG_EXAMPLE_CONNECT_ETHERNET
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include <lwip/netdb.h>
#include <arpa/inet.h>
#include "esp_err.h"


#if CONFIG_ESP32_SYSTEM_HW_NETWORK

namespace Device {
namespace Hardware {
namespace Network {

class Network {
public:
	Network();
	virtual ~Network();
};


#define HOST_IP_SIZE 128

#ifdef CONFIG_EXAMPLE_CONNECT_ETHERNET
#define EXAMPLE_INTERFACE get_example_netif()
#endif

#ifdef CONFIG_EXAMPLE_CONNECT_WIFI
#define EXAMPLE_INTERFACE get_example_netif()
#endif

#if !defined (CONFIG_EXAMPLE_CONNECT_ETHERNET) && !defined (CONFIG_EXAMPLE_CONNECT_WIFI)
// This is useful for some tests which do not need a network connection
#define EXAMPLE_INTERFACE NULL
#endif



/**
 * @brief Configure Wi-Fi or Ethernet, connect, wait for IP
 *
 * This all-in-one helper function is used in protocols examples to
 * reduce the amount of boilerplate in the example.
 *
 * It is not intended to be used in real world applications.
 * See examples under examples/wifi/getting_started/ and examples/ethernet/
 * for more complete Wi-Fi or Ethernet initialization code.
 *
 * Read "Establishing Wi-Fi or Ethernet Connection" section in
 * examples/protocols/README.md for more information about this function.
 *
 * @return ESP_OK on successful connection
 */
esp_err_t example_connect(void);

/**
 * Counterpart to example_connect, de-initializes Wi-Fi or Ethernet
 */
esp_err_t example_disconnect(void);

/**
 * @brief Returns esp-netif pointer created by example_connect()
 *
 * @note If multiple interfaces active at once, this API return NULL
 * In that case the get_example_netif_from_desc() should be used
 * to get esp-netif pointer based on interface description
 */
esp_netif_t *get_example_netif(void);

/**
 * @brief Returns esp-netif pointer created by example_connect() described by
 * the supplied desc field
 *
 * @param desc Textual interface of created network interface, for example "sta"
 * indicate default WiFi station, "eth" default Ethernet interface.
 *
 */
esp_netif_t *get_example_netif_from_desc(const char *desc);


#if CONFIG_MB_SLAVE_IP_FROM_STDIN
/**
 * @brief Configure stdin and stdout to use blocking I/O
 *
 * This helper function is used in ASIO examples. It wraps installing the
 * UART driver and configuring VFS layer to use UART driver for console I/O.
 */
esp_err_t example_configure_stdin_stdout(void);

/**
 * @brief Read and evaluate IP address from stdin
 *
 * This API reads stdin and parses the input address using getaddrinfo()
 * to fill in struct sockaddr_storage (for both IPv4 and IPv6) used to open
 * a socket. IP protocol is guessed from the IP address string.
 *
 * @param[in] port port number of expected connection
 * @param[in] sock_type expected protocol: SOCK_STREAM or SOCK_DGRAM
 * @param[out] ip_protocol resultant IP protocol: IPPROTO_IP or IPPROTO_IP6
 * @param[out] addr_family resultant address family: AF_INET or AF_INET6
 * @param[out] dest_addr sockaddr_storage structure (for both IPv4 and IPv6)
 * @return ESP_OK on success, ESP_FAIL otherwise
 */
esp_err_t get_addr_from_stdin(int port, int sock_type,
                              int *ip_protocol,
                              int *addr_family,
                              struct sockaddr_storage *dest_addr);
#endif /* CONFIG_MB_SLAVE_IP_FROM_STDIN */

} /* namespace Network */
} /* namespace Hardware */
} /* namespace Device */

#endif /* CONFIG_ESP32_SYSTEM_HW_NETWORK */

#endif /* COMPONENTS_ESP32_SYSTEM_EXTENSIONS_INCLUDE_NETWORK_H_ */

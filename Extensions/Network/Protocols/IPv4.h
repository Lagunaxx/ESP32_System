/*
 * IPv4.h
 *
 *  Created on: 23 апр. 2022 г.
 *      Author: producer
 */

#ifndef COMPONENTS_ESP32_SYSTEM_EXTENSIONS_NETWORK_PROTOCOLS_IPV4_H_
#define COMPONENTS_ESP32_SYSTEM_EXTENSIONS_NETWORK_PROTOCOLS_IPV4_H_

#include <IPAddress.h>

#include "ESP32_System.h"

namespace Device {
namespace Hardware {
namespace Network {

class class_IPv4 {
public:
	class_IPv4();
	virtual ~class_IPv4();


	static IPAddress calculateNetworkID(IPAddress ip, IPAddress subnet);
	static IPAddress calculateBroadcast(IPAddress ip, IPAddress subnet);
	static uint8_t calculateSubnetCIDR(IPAddress subnetMask);

    IPAddress localIP();
    IPAddress subnetMask();
    IPAddress gatewayIP(); // ToDo: make routing class
    IPAddress dnsIP(uint8_t dns_no = 0);

    IPAddress broadcastIP();
    IPAddress networkID();
    uint8_t subnetCIDR();

    bool enableIpV6();
    IPv6Address localIPv6();


};

} /* namespace Network */
} /* namespace Hardware */
} /* namespace Device */

#endif /* COMPONENTS_ESP32_SYSTEM_EXTENSIONS_NETWORK_PROTOCOLS_IPV4_H_ */

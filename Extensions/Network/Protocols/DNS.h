/*
 * DNS.h
 *
 *  Created on: 23 апр. 2022 г.
 *      Author: producer
 */

#ifndef COMPONENTS_ESP32_SYSTEM_EXTENSIONS_NETWORK_PROTOCOLS_DNS_H_
#define COMPONENTS_ESP32_SYSTEM_EXTENSIONS_NETWORK_PROTOCOLS_DNS_H_

namespace Device {
namespace Hardware {
namespace Network {

class class_DNS {
public:
	class_DNS();
	virtual ~class_DNS();

	static int hostByName(const char *aHostname, IPAddress &aResult);

};

} /* namespace Network */
} /* namespace Hardware */
} /* namespace Device */

#endif /* COMPONENTS_ESP32_SYSTEM_EXTENSIONS_NETWORK_PROTOCOLS_DNS_H_ */

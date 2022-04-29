/*
 * Host.h
 *
 *  Created on: 23 апр. 2022 г.
 *      Author: producer
 */

#ifndef COMPONENTS_ESP32_SYSTEM_EXTENSIONS_NETWORK_HOST_H_
#define COMPONENTS_ESP32_SYSTEM_EXTENSIONS_NETWORK_HOST_H_


#include "ESP32_System.h"

namespace Device {
namespace Hardware {
namespace Network {

class class_Host {
public:
	class_Host();
	virtual ~class_Host();

	static const char * getHostname();
	static bool setHostname(const char * hostname);
	static bool hostname(const String& aHostname) { return setHostname(aHostname.c_str()); }

};

} /* namespace Network */
} /* namespace Hardware */
} /* namespace Device */

#endif /* COMPONENTS_ESP32_SYSTEM_EXTENSIONS_NETWORK_HOST_H_ */

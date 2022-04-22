/*
 * WiFi.h
 *
 *  Created on: 22 апр. 2022 г.
 *      Author: producer
 */

#ifndef COMPONENTS_ESP32_SYSTEM_EXTENSIONS_WIFI_H_
#define COMPONENTS_ESP32_SYSTEM_EXTENSIONS_WIFI_H_

#include "ESP32_System.h"

#if CONFIG_ESP32_SYSTEM_HW_NETWORK_WIFI

namespace Device {
namespace Hardware {
namespace Network {

class WiFi {
public:
	WiFi();
	virtual ~WiFi();
};

} /* namespace Network */
} /* namespace Hardware */
} /* namespace Device */


#endif /* CONFIG_ESP32_SYSTEM_HW_NETWORK_WIFI */
#endif /* COMPONENTS_ESP32_SYSTEM_EXTENSIONS_WIFI_H_ */

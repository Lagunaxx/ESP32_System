/*
 * WiFi.h
 *
 *  Created on: 22 апр. 2022 г.
 *      Author: producer
 */

#ifndef COMPONENTS_ESP32_SYSTEM_EXTENSIONS_WIFI_H_
#define COMPONENTS_ESP32_SYSTEM_EXTENSIONS_WIFI_H_

// below from arduino-esp
#include <stdint.h>

#include "Print.h"
#include "IPAddress.h"
#include "IPv6Address.h"
/*
#include "WiFiType.h"
#include "WiFiSTA.h"
#include "WiFiAP.h"
#include "WiFiScan.h"
#include "WiFiClient.h"
#include "WiFiServer.h"
#include "WiFiUdp.h"
*/

/*
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
*/
#include <esp_err.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include "ESP32_System.h"
//#include "WiFiGeneric.h" // +




#if CONFIG_ESP32_SYSTEM_HW_NETWORK_WIFI

namespace Device {
namespace Hardware {
namespace Network {

#define WIFI_TAG "[WiFi]"

#define WIFI_check_esp_err_t(result,rettrue,retfalse)		if (result != ESP_OK){\
																ESP_LOGW(WIFI_TAG,"error! %s", esp_err_to_name(result));\
																return retfalse;\
															}\
															return rettrue;

// : public class_WiFiGeneric, public WiFiSTAClass, public WiFiScanClass, public WiFiAPClass
class class_WiFi{
public:
	class_WiFi();
	virtual ~class_WiFi();




	bool	init(),		// Call first befor any WiFi API function
			destroy();		// Call at end of work with WiFi API functions

	bool	setMode(wifi_mode_t mode),
			getMode(wifi_mode_t* mode);
/*
 * esp_err_tesp_wifi_restore(void)
Restore WiFi stack persistent settings to default values.
This function will reset settings made using the following APIs:
esp_wifi_set_bandwidth,
esp_wifi_set_protocol,
esp_wifi_set_config related
esp_wifi_set_mode
 */

	bool	start(),
			stop();








};

} /* namespace Network */
} /* namespace Hardware */
} /* namespace Device */


#endif /* CONFIG_ESP32_SYSTEM_HW_NETWORK_WIFI */
#endif /* COMPONENTS_ESP32_SYSTEM_EXTENSIONS_CWIFI_H_ */

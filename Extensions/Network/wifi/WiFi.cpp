/*
 * WiFi.cpp
 *
 *  Created on: 22 апр. 2022 г.
 *      Author: producer
 */

#include "WiFi.h"

#if CONFIG_ESP32_SYSTEM_HW_NETWORK_WIFI

namespace Device {
namespace Hardware {
namespace Network {

class_WiFi::class_WiFi() {
	// TODO Auto-generated constructor stub


}

class_WiFi::~class_WiFi() {
	// TODO Auto-generated destructor stub
}

/***************************************************************************************
** Function name:			init
** Description :			Initialize WiFi. mast be called first befor using any WiFi API
** Return:			true	- if succesfully
** 					false	- if errors was due initialization (not initialized)
***************************************************************************************/
bool class_WiFi::init(){
	wifi_init_config_t wf_config = WIFI_INIT_CONFIG_DEFAULT(); // @suppress("Invalid arguments")
	esp_err_t result = esp_wifi_init(&wf_config);
	WIFI_check_esp_err_t(result,true,false); // @suppress("Invalid arguments")
}

/***************************************************************************************
** Function name:			destroy
** Description :			destroy WiFi. mast be called last after using WiFi API.
** 							Oposite to init()-function, frees resources.
** Return:			true	- if succesfully
** 					false	- if errors was happened
***************************************************************************************/
bool class_WiFi::destroy(){
	esp_err_t result = esp_wifi_deinit();
	WIFI_check_esp_err_t(result,true,false); // @suppress("Invalid arguments")
}

/***************************************************************************************
** Function name:			setMode
** Description :			Set mode for WiFi.
** Args:
**			mode - values:
**					WIFI_MODE_NULL, WIFI_MODE_STA, WIFI_MODE_AP, WIFI_MODE_APSTA,
**					WIFI_MODE_MAX
**
** Return:			true	- if succesfully
** 					false	- if errors was happened
***************************************************************************************/
bool class_WiFi::setMode(wifi_mode_t mode){
	esp_err_t result = esp_wifi_set_mode(mode);
	WIFI_check_esp_err_t(result,true,false); // @suppress("Invalid arguments")
}

/***************************************************************************************
** Function name:			setMode
** Description :			Set mode for WiFi.
** Args:
**			mode - values:
**					WIFI_MODE_NULL, WIFI_MODE_STA, WIFI_MODE_AP, WIFI_MODE_APSTA,
**					WIFI_MODE_MAX
**
** Return:			true	- if succesfully
** 					false	- if errors was happened
***************************************************************************************/

bool class_WiFi::getMode(wifi_mode_t* mode){
	esp_err_t result = esp_wifi_get_mode(mode);
	WIFI_check_esp_err_t(result,true,false); // @suppress("Invalid arguments")
}

/***************************************************************************************
** Function name:			start
** Description :			Start WiFi according to current configuration
** 							If mode is WIFI_MODE_STA, it create station control block
** 							and start station If mode is WIFI_MODE_AP, it create soft-AP
** 							control block and start soft-AP If mode is WIFI_MODE_APSTA,
** 							it create soft-AP and station control block and start soft-AP
** 							and station.
** Return:			true	- if succesfully
** 					false	- if errors was happened
***************************************************************************************/
bool class_WiFi::start(){
	esp_err_t result = esp_wifi_start();
	WIFI_check_esp_err_t(result,true,false); // @suppress("Invalid arguments")
}

/***************************************************************************************
** Function name:			stop
** Description :			Stop WiFi If mode is WIFI_MODE_STA, it stop station and free
** 							station control block If mode is WIFI_MODE_AP, it stop soft-AP
** 							and free soft-AP control block If mode is WIFI_MODE_APSTA,
** 							it stop station/soft-AP and free station/soft-AP control block.
** Return:			true	- if succesfully
** 					false	- if errors was happened
***************************************************************************************/
bool class_WiFi::stop(){
	esp_err_t result = esp_wifi_stop();
	WIFI_check_esp_err_t(result,true,false); // @suppress("Invalid arguments")
}


} /* namespace Network */
} /* namespace Hardware */
} /* namespace Device */

#endif /* CONFIG_ESP32_SYSTEM_HW_NETWORK_WIFI */


/***************************************************************************************
** Function name:			name
** Description :			desc
** Args:
**			arg1 - value
***************************************************************************************/

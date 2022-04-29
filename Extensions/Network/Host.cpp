/*
 * Host.cpp
 *
 *  Created on: 23 апр. 2022 г.
 *      Author: producer
 */

#include "Host.h"

namespace Device {
namespace Hardware {
namespace Network {

class_Host::class_Host() {
	// TODO Auto-generated constructor stub

}

class_Host::~class_Host() {
	// TODO Auto-generated destructor stub
}

static char default_hostname[32] = {0,};
static const char * get_esp_netif_hostname(){
	if(default_hostname[0] == 0){
	    uint8_t eth_mac[6];
	    esp_wifi_get_mac((wifi_interface_t)WIFI_IF_STA, eth_mac);
	    snprintf(default_hostname, 32, "%s%02X%02X%02X", CONFIG_IDF_TARGET "-", eth_mac[3], eth_mac[4], eth_mac[5]);
	}
	return (const char *)default_hostname;
}

static void set_esp_netif_hostname(const char * name){
	if(name){
		snprintf(default_hostname, 32, "%s", name);
	}
}

const char * class_Host::getHostname()
{
    return get_esp_netif_hostname();
}

bool class_Host::setHostname(const char * hostname)
{
    set_esp_netif_hostname(hostname);
    return true;
}




} /* namespace Network */
} /* namespace Hardware */
} /* namespace Device */

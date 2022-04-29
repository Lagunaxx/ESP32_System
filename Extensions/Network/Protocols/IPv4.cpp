/*
 * IPv4.cpp
 *
 *  Created on: 23 апр. 2022 г.
 *      Author: producer
 */

#include "IPv4.h"

namespace Device {
namespace Hardware {
namespace Network {

class_IPv4::class_IPv4() {
	// TODO Auto-generated constructor stub

}

class_IPv4::~class_IPv4() {
	// TODO Auto-generated destructor stub
}

IPAddress class_IPv4::calculateNetworkID(IPAddress ip, IPAddress subnet) {
	IPAddress networkID;

	for (size_t i = 0; i < 4; i++)
		networkID[i] = subnet[i] & ip[i];

	return networkID;
}

IPAddress class_IPv4::calculateBroadcast(IPAddress ip, IPAddress subnet) {
    IPAddress broadcastIp;

    for (int i = 0; i < 4; i++)
        broadcastIp[i] = ~subnet[i] | ip[i];

    return broadcastIp;
}

uint8_t class_IPv4::calculateSubnetCIDR(IPAddress subnetMask) {
	uint8_t CIDR = 0;

	for (uint8_t i = 0; i < 4; i++) {
		if (subnetMask[i] == 0x80)  // 128
			CIDR += 1;
		else if (subnetMask[i] == 0xC0)  // 192
			CIDR += 2;
		else if (subnetMask[i] == 0xE0)  // 224
			CIDR += 3;
		else if (subnetMask[i] == 0xF0)  // 242
			CIDR += 4;
		else if (subnetMask[i] == 0xF8)  // 248
			CIDR += 5;
		else if (subnetMask[i] == 0xFC)  // 252
			CIDR += 6;
		else if (subnetMask[i] == 0xFE)  // 254
			CIDR += 7;
		else if (subnetMask[i] == 0xFF)  // 255
			CIDR += 8;
	}

	return CIDR;
}

/**
 * Get the station interface IP address.
 * @return IPAddress station IP
 */
IPAddress class_IPv4::localIP()
{
    if(WiFiGenericClass::getMode() == WIFI_MODE_NULL){
        return IPAddress();
    }
	esp_netif_ip_info_t ip;
    if(esp_netif_get_ip_info(get_esp_interface_netif(ESP_IF_WIFI_STA), &ip) != ESP_OK){
    	log_e("Netif Get IP Failed!");
    	return IPAddress();
    }
    return IPAddress(ip.ip.addr);
}

/**
 * Get the interface subnet mask address.
 * @return IPAddress subnetMask
 */
IPAddress class_IPv4::subnetMask()
{
    if(WiFiGenericClass::getMode() == WIFI_MODE_NULL){
        return IPAddress();
    }
	esp_netif_ip_info_t ip;
    if(esp_netif_get_ip_info(get_esp_interface_netif(ESP_IF_WIFI_STA), &ip) != ESP_OK){
    	log_e("Netif Get IP Failed!");
    	return IPAddress();
    }
    return IPAddress(ip.netmask.addr);
}

/**
 * Get the gateway ip address.
 * @return IPAddress gatewayIP
 */
// ToDo: mopving to IP, need make routing class
IPAddress class_IPv4::gatewayIP()
{
    if(WiFiGenericClass::getMode() == WIFI_MODE_NULL){
        return IPAddress();
    }
	esp_netif_ip_info_t ip;
    if(esp_netif_get_ip_info(get_esp_interface_netif(ESP_IF_WIFI_STA), &ip) != ESP_OK){
    	log_e("Netif Get IP Failed!");
    	return IPAddress();
    }
    return IPAddress(ip.gw.addr);
}


/**
 * Get the DNS ip address.
 * @param dns_no
 * @return IPAddress DNS Server IP
 */
IPAddress class_IPv4::dnsIP(uint8_t dns_no)
{
    if(WiFiGenericClass::getMode() == WIFI_MODE_NULL){
        return IPAddress();
    }
    const ip_addr_t * dns_ip = dns_getserver(dns_no);
    return IPAddress(dns_ip->u_addr.ip4.addr);
}

/**
 * Get the broadcast ip address.
 * @return IPAddress broadcastIP
 */
IPAddress class_IPv4::broadcastIP()
{
    if(WiFiGenericClass::getMode() == WIFI_MODE_NULL){
        return IPAddress();
    }
	esp_netif_ip_info_t ip;
    if(esp_netif_get_ip_info(get_esp_interface_netif(ESP_IF_WIFI_STA), &ip) != ESP_OK){
    	log_e("Netif Get IP Failed!");
    	return IPAddress();
    }
    return WiFiGenericClass::calculateBroadcast(IPAddress(ip.gw.addr), IPAddress(ip.netmask.addr));
}

/**
 * Get the network id.
 * @return IPAddress networkID
 */
IPAddress class_IPv4::networkID()
{
    if(WiFiGenericClass::getMode() == WIFI_MODE_NULL){
        return IPAddress();
    }
	esp_netif_ip_info_t ip;
    if(esp_netif_get_ip_info(get_esp_interface_netif(ESP_IF_WIFI_STA), &ip) != ESP_OK){
    	log_e("Netif Get IP Failed!");
    	return IPAddress();
    }
    return WiFiGenericClass::calculateNetworkID(IPAddress(ip.gw.addr), IPAddress(ip.netmask.addr));
}

/**
 * Get the subnet CIDR.
 * @return uint8_t subnetCIDR
 */
uint8_t class_IPv4::subnetCIDR()
{
    if(WiFiGenericClass::getMode() == WIFI_MODE_NULL){
        return (uint8_t)0;
    }
	esp_netif_ip_info_t ip;
    if(esp_netif_get_ip_info(get_esp_interface_netif(ESP_IF_WIFI_STA), &ip) != ESP_OK){
    	log_e("Netif Get IP Failed!");
    	return IPAddress();
    }
    return WiFiGenericClass::calculateSubnetCIDR(IPAddress(ip.netmask.addr));
}

/**
 * Enable IPv6 on the station interface.
 * @return true on success
 */
bool class_IPv4::enableIpV6()
{
    if(WiFiGenericClass::getMode() == WIFI_MODE_NULL){
        return false;
    }
    return esp_netif_create_ip6_linklocal(get_esp_interface_netif(ESP_IF_WIFI_STA)) == ESP_OK;
}

/**
 * Get the station interface IPv6 address.
 * @return IPv6Address
 */
IPv6Address class_IPv4::localIPv6()
{
	esp_ip6_addr_t addr;
    if(WiFiGenericClass::getMode() == WIFI_MODE_NULL){
        return IPv6Address();
    }
    if(esp_netif_get_ip6_linklocal(get_esp_interface_netif(ESP_IF_WIFI_STA), &addr)) {
        return IPv6Address();
    }
    return IPv6Address(addr.addr);
}

} /* namespace Network */
} /* namespace Hardware */
} /* namespace Device */

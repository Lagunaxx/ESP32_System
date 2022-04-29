/*
 * DNS.cpp
 *
 *  Created on: 23 апр. 2022 г.
 *      Author: producer
 */

#include "DNS.h"

namespace Device {
namespace Hardware {
namespace Network {

class_DNS::class_DNS() {
	// TODO Auto-generated constructor stub

}

class_DNS::~class_DNS() {
	// TODO Auto-generated destructor stub
}

/**
 * DNS callback
 * @param name
 * @param ipaddr
 * @param callback_arg
 */
static void wifi_dns_found_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg)
{
    if(ipaddr) {
        (*reinterpret_cast<IPAddress*>(callback_arg)) = ipaddr->u_addr.ip4.addr;
    }
    xEventGroupSetBits(_arduino_event_group, WIFI_DNS_DONE_BIT);
}

/**
 * Resolve the given hostname to an IP address.
 * @param aHostname     Name to be resolved
 * @param aResult       IPAddress structure to store the returned IP address
 * @return 1 if aIPAddrString was successfully converted to an IP address,
 *          else error code
 */
int class_WiFiGeneric::hostByName(const char* aHostname, IPAddress& aResult)
{
    ip_addr_t addr;
    aResult = static_cast<uint32_t>(0);
    waitStatusBits(WIFI_DNS_IDLE_BIT, 16000);
    clearStatusBits(WIFI_DNS_IDLE_BIT | WIFI_DNS_DONE_BIT);
    err_t err = dns_gethostbyname(aHostname, &addr, &wifi_dns_found_callback, &aResult);
    if(err == ERR_OK && addr.u_addr.ip4.addr) {
        aResult = addr.u_addr.ip4.addr;
    } else if(err == ERR_INPROGRESS) {
        waitStatusBits(WIFI_DNS_DONE_BIT, 15000);  //real internal timeout in lwip library is 14[s]
        clearStatusBits(WIFI_DNS_DONE_BIT);
    }
    setStatusBits(WIFI_DNS_IDLE_BIT);
    if((uint32_t)aResult == 0){
        log_e("DNS Failed for %s", aHostname);
    }
    return (uint32_t)aResult != 0;
}



} /* namespace Network */
} /* namespace Hardware */
} /* namespace Device */

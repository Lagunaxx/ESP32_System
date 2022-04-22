/*
 * ModbasUART.h
 *
 *  Created on: 22 апр. 2022 г.
 *      Author: producer
 */

#ifndef COMPONENTS_ESP32_SYSTEM_EXTENSIONS_MODBASUART_H_
#define COMPONENTS_ESP32_SYSTEM_EXTENSIONS_MODBASUART_H_

#include "ESP32_System.h"

#if CONFIG_ESP32_SYSTEM_HW_MODBUS_UART


namespace Device {
namespace Hardware {

#define MB_PORT_NUM     (CONFIG_MB_UART_PORT_NUM)   // Number of UART port used for Modbus connection
#define MB_DEV_SPEED    (CONFIG_MB_UART_BAUD_RATE)  // The communication speed of the UART

// Note: Some pins on target chip cannot be assigned for UART communication.
// See UART documentation for selected board and target to configure pins using Kconfig.

// The number of parameters that intended to be used in the particular control process
#define MASTER_MAX_CIDS num_device_parameters // ToDo: fix

// Number of reading of parameters from slave
#define MASTER_MAX_RETRY 30

// Timeout to update cid over Modbus
#define UPDATE_CIDS_TIMEOUT_MS          (500)
#define UPDATE_CIDS_TIMEOUT_TICS        (UPDATE_CIDS_TIMEOUT_MS / portTICK_RATE_MS)

// Timeout between polls
#define POLL_TIMEOUT_MS                 (1)
#define POLL_TIMEOUT_TICS               (POLL_TIMEOUT_MS / portTICK_RATE_MS)

#define MASTER_TAG "MASTER_TEST"

#define MASTER_CHECK(a, ret_val, str, ...) \
    if (!(a)) { \
        ESP_LOGE(MASTER_TAG, "%s(%u): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return (ret_val); \
    }

#define MB_SLAVE_ADDR   (CONFIG_MB_SLAVE_ADDR)      // The address of device in Modbus network // for slave


class Modbas_UART {
public:
	Modbas_UART();
	virtual ~Modbas_UART();


	/************************************************************************************************************
	 * 																											*
	 * 									Serial modbus															*
	 *  																										*
	 ************************************************************************************************************/

	// master







	static void master_uart_operation_func(void *arg);
	static esp_err_t master_init(void);
	void run_master(void);





	// slave



	void run_slave(void);

};

} /* namespace Hardware */
} /* namespace Device */


#endif /* CONFIG_ESP32_SYSTEM_HW_MODBUS_UART */
#endif /* COMPONENTS_ESP32_SYSTEM_EXTENSIONS_MODBASUART_H_ */

/*
 * Hardware.h
 *
 *  Created on: 13 апр. 2022 г.
 *      Author: producer
 *
 *  Extension defines root driver ( run() ) and template class for all hardwares.
 *
 */

// Stop fonts etc being loaded multiple times
#ifndef ESP32_SYSTEM_H_
#define ESP32_SYSTEM_H_

#define ESP32_SYSTEM_VERSION "0.0.1"

#include <Arduino.h>
#include <SPI.h>
#include <pgmspace.h>

/*********************************************************
 * Default initialization                                *
 *********************************************************/
#ifdef ESP32
#include <soc/spi_reg.h>

#include "sdkconfig.h"

#ifdef USE_HSPI_PORT
			#define SPI_PORT HSPI
		#else
#define SPI_PORT VSPI
#endif // USE_HSPI_PORT
#endif //ESP32

//#include <pthread.h>
//#include "esp_pthread.h"

// Add extensions
#if CONFIG_ESP32_SYSTEM_HW_NETWORK
#include "Network.h"

#if CONFIG_ESP32_SYSTEM_HW_NETWORK_WIFI
#include "WiFi.h"
#endif

#endif /* CONFIG_ESP32_SYSTEM_HW_NETWORK */


#if CONFIG_ESP32_SYSTEM_HW_MODBUS_UART
#include "ModbusUART.h"
#endif // CONFIG_ESP32_SYSTEM_HW_MODBUS_UART
#if CONFIG_ESP32_SYSTEM_HW_MODBUS_TCP
#include "ModbusTCP.h"
#endif // CONFIG_ESP32_SYSTEM_HW_MODBUS_TCP


// Defining default system messages values
#define MSG_SYS_NOP			0	// no operation

// Defining type for HW
#define HW_TYPE_NOP		0	// not defined
#define HW_TYPE_DI		1	// digital input
#define HW_TYPE_DO		2	// digital output
#define HW_TYPE_AI		3	// analog input

// Define pins
#define GPIO0	0
#define GPIO1	1
#define GPIO2	2
#define GPIO3	3
#define GPIO4	4
#define GPIO5	5
#define GPIO6	6
#define GPIO7	7
#define GPIO8	8
#define GPIO9	9
#define GPIO10	10
#define GPIO11	11
#define GPIO12	12
#define GPIO13	13
#define GPIO14	14
#define GPIO15	15
#define GPIO16	16
#define GPIO17	17
#define GPIO18	18
#define GPIO19	19
#define GPIO20	20
#define GPIO21	21
#define GPIO22	22
#define GPIO23	23
#define GPIO24	24
#define GPIO25	25
#define GPIO26	26
#define GPIO27	27
#define GPIO28	28
#define GPIO29	29
#define GPIO30	30
#define GPIO31	31
#define GPIO32	32
#define GPIO33	33
#define GPIO34	34
#define GPIO35	35
#define GPIO36	36
#define GPIO37	37
#define GPIO38	38
#define GPIO39	39
#define GPIO40	40

#define GPIO_NOTDEFINED 255


namespace Device {
namespace Hardware {

// namespace Network {}

typedef struct {
	uint8_t pin;
	uint8_t deviceType;
	void * data;
} t_Data;

typedef struct {
	uint8_t ID;
	uint8_t deviceType;
	uint8_t pin;
	void * deviceHandler;
	void (* callbackHandler)(t_Data *);
} t_Device;

/*********************************************************************************
*	Class:	Hardware
*	Description:
*		Bus initializes itself (Bus obect)
*		Use dBus->run(func) in main loop to run main bus. Where func - pointer to
*		handler function.
*		To add device to bus use add() functions.
*		When need to send data to some device - call Bus->msg(params).
*
*	Usage:
*		First initialize extension with:
*			Device::Hardware::init();
*		Add built-in  "devices" (like pins, etc):
*			Device::Hardware::Bus->add***(params, void callbackHandler(t_Data*)) functions
*		callbackHandler function mast be defined like:
*			void callbackHandler(Device::Hardware::t_Data * data);
*		where callbackHandler - any name you wish.
*
*	Example:
*
*		void handler(Device::Hardware::t_Data * data){
*			// test hardware
*			int rvalue;
*			rvalue = (int)(*(int *)data->data);
*		}
*
*		void setup() {
*		  ...
*		  Device::Hardware::init();
*		  Device::Hardware::Bus->addDI(GPIO35, &handler);
*		  ...
*		}
*********************************************************************************/
class Hardware {

public:
	Hardware();
	virtual ~Hardware();

	void run();		// Run root driver (hwControl())
	uint8_t addDI(uint8_t pin, void callbackHandler(t_Data*));	// Add new DI device
	uint8_t addAI(uint8_t pin, void callbackHandler(t_Data*));	// Add new AI device
	virtual uint8_t msg(uint8_t deviceID, uint8_t msg, void * payload); // Send data to device

protected:

	uint8_t hwControl();

	t_Device *Devices;
};

extern Hardware* Bus;
bool init();
bool remove();
void Handle();

} /* namespace Hardware */
} /* namespace Device */

#endif /* COMPONENTS_ESP32_SPIDISPLAY_EXTENSIONS_HARDWARE_HARDWARE_H_ */

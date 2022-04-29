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

#include "esp_event.h"
#include "esp_event_base.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "projdefs.h"

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

//#include "Extensions/IO.h"

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

enum{
	DEVICE_HARDWARE_EVENT_DELETE = 1,

	DEVICE_HARDWARE_EVENT_LAST
};

// namespace Network {}
enum {
    TASK_SYSTEM_EVENT                     // raised during an event of the loop within the task
};

typedef struct {
	uint8_t pin;
	uint8_t deviceType;
	void * data;
} t_Data;

typedef struct {
	uint8_t id;
	uint8_t deviceType;
	uint8_t pin;
	void * deviceHandler;
	void (* callbackHandler)(t_Data *);
} t_Device;

typedef struct {	// when device handling class initiates it mast call static function for registering itself.
	uint32_t	id;				// id of registered class
	uint32_t	event_id;		// event-id for registered device handling class (may be '#define ADD_DI 1' (local for every handler-class)
	esp_event_loop_args_t eventLoopArgs; // register esp_event_loop_args_t for use in esp_event_loop_create(...)
//	unsigned int	stackSize;	// defines stack size
	void *		handleClass;	// pointer to handling class. (may be remmove this rule: mast have 'Hardware' as parent class,)
								// mast implement 'static void handle' function, 'static void loop', and other like 'add' function and etc.
	void *		eventHandlerArg;// it will be used as 'event_handler_arg' in 'esp_event_handler_instance_register_with' and
								// mast be used in 'handler' as same to know from what event-system event came.
	TaskFunction_t fn_loop;
//	const char* description;	// short description (naming) of class (like "DI", "AO", "WiFi", etc). moving to esp_event_loop_args_t.task_name
} t_regHW;

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

	static void handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
//	virtual bool add(uint8_t type, void* data, Hardware* hw_call, esp_event_loop_args_t* return_elArgs); //
	static void loop(); // Every device works in this loop and calls every handler, who adds this device, when event occures.

//	static void init(); // Initialize static variables

//	static void HWDriver(); // This driver will run all hw-loops



/*	void run();		// Run root driver (hwControl())


	virtual uint8_t msg(uint8_t deviceID, uint8_t msg, void * payload); // Send data to device
*/
//protected:

	 uint32_t	registerType(t_regHW* reg);		/* Register device's handler class.
													* Device class mast initialize 'reg' and it mast be
													* available due device's handler class works.
													* When device's handler class destroys - it mast call
													* removing function of Hardware, where reg will be freed.
													*/



private:
	 uint32_t	numberOfHandlerClasses,		// Number of registered handler classes
					maxNumberOfHandlerClasses; // Maximum of above
	 t_regHW**	HandlerClasses;					// Place for storing handler classes




// ToDo: this used in Device driver (handler) to register callbacks and call them when device-event happened.
/*	Hardware** hw_recall;						// Store pointers for every registered callback-class (callback class mast have function handler()
	uint16_t	numberOfHWRecall = 0,			// Number of registered classes with callback function
				Max_numberOfHWRecall = 65530;	// Maximum number of classes to register


	uint8_t hwControl();



	ESP_EVENT_DEFINE_BASE(TASK_EVENTS);

	static void event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
	esp_event_loop_handle_t event_loop_handler;
	esp_event_loop_args_t event_args = { // @suppress("Invalid arguments")
	        .queue_size = 15,
	        .task_name = "ESP32_System handler",
	        .task_priority = uxTaskPriorityGet(NULL), // @suppress("Ambiguous problem")
	        .task_stack_size = 2048,
	        .task_core_id = tskNO_AFFINITY
	};
// */
};



extern Hardware* Bus;
bool init();
bool remove();
void Handle();

} /* namespace Hardware */
} /* namespace Device */

#endif /* COMPONENTS_ESP32_SPIDISPLAY_EXTENSIONS_HARDWARE_HARDWARE_H_ */

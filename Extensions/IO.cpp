/*
 * IO.cpp
 *
 *  Created on: 25 апр. 2022 г.
 *      Author: producer
 */

#include "IO.h"

namespace Device {
namespace Hardware {

ESP_EVENT_DECLARE_BASE(TASK_EVENTS_HW_IO);

/***************************************************************************************
** Function name:			class_IO
** Description :			Constructor
***************************************************************************************/
class_IO::class_IO() {
	delay = 10; // delay by default (ms)

	Devices = 0;
	Devices = new(t_Device[40]);
	if(Devices){
		for(uint8_t id=0; id<40; id++) {
			Devices[id].id = id;
			Devices[id].callbackHandler = 0;
			Devices[id].deviceHandler = 0;
			Devices[id].deviceType = HW_TYPE_NOP;
			Devices[id].pin = GPIO_NOTDEFINED;
		}
	}


	t_regHW *reg = new t_regHW();
	reg->id = 0;
	reg->handleClass = (void *) this; // mast be same for any registering class in Hardware bus to work with _loop
	reg->event_id = 0; // event_id here
	reg->eventLoopArgs.queue_size = 30; // queue size here
	reg->eventLoopArgs.task_core_id = 0; // core. may be 0, 1 or tskNO_AFFINITY
	reg->eventLoopArgs.task_name = "IO";
	reg->eventLoopArgs.task_priority = uxTaskPriorityGet(NULL);
	reg->eventLoopArgs.task_stack_size = 1024; // stack size
	reg->eventHandlerArg = (void *) this;
	reg->fn_loop = &IO_loop;
//Serial.printf ("HW IO registering...\n");
	Bus->registerType(reg);
}

/***************************************************************************************
** Function name:			~class_IO
** Description :			Destructor
***************************************************************************************/
class_IO::~class_IO() {
	// TODO Auto-generated destructor stub
}

/***************************************************************************************
** Function name:			IO_loop
** Description :			main loop. It starts by Hardware Bus when registered this class
** Args:
**			args - pointer to reg->handleClass (this class)
***************************************************************************************/
void class_IO::IO_loop(void * args){
	while(true){

		t_Data * rvalue;
		int rv;
		// Check every initiate pin

		for (uint8_t id = 0; id < 40; id++){
			if(((class_IO *)args)->Devices[id].pin != GPIO_NOTDEFINED) {
				// check pin type
				if(((class_IO *)args)->Devices[id].deviceType == HW_TYPE_DI) {
					// check input
					rvalue=new(t_Data);
					rvalue->data = malloc(sizeof(int));
					rvalue->deviceType = ((class_IO *)args)->Devices[id].deviceType;
					rvalue->pin = ((class_IO *)args)->Devices[id].pin;
					rv = digitalRead(((class_IO *)args)->Devices[id].pin);
					memcpy(rvalue->data,(const void *)&rv,sizeof(int));
					(*((class_IO *)args)->Devices[id].callbackHandler)( rvalue);
					free(rvalue->data);
					delete(rvalue);
				}
				if(((class_IO *)args)->Devices[id].deviceType == HW_TYPE_AI) {
					// check input
					rvalue=new(t_Data);
					rvalue->data = malloc(sizeof(int));
					rvalue->deviceType = ((class_IO *)args)->Devices[id].deviceType;
					rvalue->pin = ((class_IO *)args)->Devices[id].pin;
					rv = analogRead(((class_IO *)args)->Devices[id].pin);
					memcpy(rvalue->data,(const void *)&rv,sizeof(int));
					(*((class_IO *)args)->Devices[id].callbackHandler)( rvalue);
					free(rvalue->data);
					delete(rvalue);
				}

			}

		}


		vTaskDelay((unsigned int) (((class_IO *)args)->delay/portTICK_PERIOD_MS));
	}
}

/***************************************************************************************
** Function name:			addXX
** Description :			initializes new device (pin), where XX = DI, AI, DO, AO
** Args:
**			arg1 - value
**
** Return:	id, or 0 if error
***************************************************************************************/
// *
uint8_t class_IO::addDI(uint8_t pin, void callbackHandler(t_Data*)) {
	uint8_t id=0;

	if (!callbackHandler) return 0; // check for null-pointer

	while (id<40) {
		if (Devices[id].pin == pin) return 0; // if pin initialized then exit
		id++;
	}

	id = 0;
	while (true) {
		if (Devices[id].pin == GPIO_NOTDEFINED)break;
		id++;
		if(id == 40) return 0; // all pins are buizy
	}

	Devices[id].pin = pin;
	Devices[id].deviceType = HW_TYPE_DI;
	Devices[id].callbackHandler = callbackHandler;
	pinMode(pin, INPUT);

	return id;
}
uint8_t class_IO::addAI(uint8_t pin, void callbackHandler(t_Data*)) {
	uint8_t id=0;

	if (!callbackHandler) return 0; // check for null-pointer

	while (id<40) {
		if (Devices[id].pin == pin) return 0; // if pin initialized then exit
		id++;
	}

	id = 0;
	while (true) {
		if (Devices[id].pin == GPIO_NOTDEFINED)break;
		id++;
		if(id == 40) return 0; // all pins are buizy
	}

	Devices[id].pin = pin;
	Devices[id].deviceType = HW_TYPE_AI;
	Devices[id].callbackHandler = callbackHandler;
	pinMode(pin, INPUT);

	return id;
}
// */

} /* namespace Hardware */
} /* namespace Device */

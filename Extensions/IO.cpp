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

class_IO::class_IO() {
	t_regHW *reg = new t_regHW();
	reg->id = 0;
	reg->handleClass = (void *) this;
	reg->event_id = 0; // event_id here
	reg->eventLoopArgs.queue_size = 30; // queue size here
	reg->eventLoopArgs.task_core_id = 0; // core. may be 0, 1 or tskNO_AFFINITY
	reg->eventLoopArgs.task_name = "IO";
	reg->eventLoopArgs.task_priority = uxTaskPriorityGet(NULL);
	reg->eventLoopArgs.task_stack_size = 1024; // stack size
	reg->eventHandlerArg = (void *) this;
	reg->fn_loop = &_loop;

	Serial.printf ("HW IO registering...\n");
	Bus->registerType(reg);
}

class_IO::~class_IO() {
	// TODO Auto-generated destructor stub
}

void class_IO::_loop(void *){
	while(true){
		Serial.printf("HWdriver IO running.\n");
		vTaskDelay((unsigned int) (1000/portTICK_PERIOD_MS));
	}
}



} /* namespace Hardware */
} /* namespace Device */

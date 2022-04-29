/*
 * Hardware.cpp
 *
 *  Created on: 13 апр. 2022 г.
 *      Author: producer
 */

#include "ESP32_System.h"

namespace Device {
namespace Hardware {

Hardware* Bus;
const char* SYSTEM_TAG = "[ESP32_System]";

void Handle(){
	delay(1);
//	Bus->run();
}
/***************************************************************************************
** Function name:           init
** Description:             initialize Hardware module
** Usage:
** 		Device::Display::Graphics::init();		// initialize Bus pointer
**
** 	Return value (bool):
** 		true - Bus initialized;
** 		false - Bus do not initialized
***************************************************************************************/
bool init(){
			//	Hardware::numberOfHandlerClasses = 0;
			//	Hardware::maxNumberOfHandlerClasses = 2147483640;
				Bus=new Hardware();
				if (Bus==0) return false;
				//Bus->init();
				return true;
			};

/***************************************************************************************
** Function name:           remove
** Description:             removes Hardware bus
** Usage:
** 		Device::Hardware::remove();	// removes Bus pointer.
**
** 	Return value (bool):
** 		true - Bus removed;
** 		false - Bus was not initialized (null-pointer)
***************************************************************************************/
 bool remove(){
	if (Bus==0) return false;
	delete(Bus);
	return true;
}


Hardware::Hardware() {
//	esp_err_t tmp_result;

	ESP_LOGI(SYSTEM_TAG, "starting..."); // @suppress("Invalid arguments")

	numberOfHandlerClasses = 0;
	maxNumberOfHandlerClasses = 2147483640;

	// ToDo: remove. test purpose:
/*
 	esp_event_loop_args_t event_args = { // @suppress("Invalid arguments")
	        .queue_size = 15,
	        .task_name = "ESP32_System handler",
	        .task_priority = uxTaskPriorityGet(NULL), // @suppress("Ambiguous problem")
	        .task_stack_size = 2048,
	        .task_core_id = tskNO_AFFINITY
	};
	BaseType_t tmp_result = xTaskCreate((TaskFunction_t)(&loop), "HWARE TASK", event_args.task_stack_size, NULL, uxTaskPriorityGet(NULL), NULL); // @suppress("Invalid arguments") // @suppress("Field cannot be resolved")
//	*/
	// older tests:
/*	Devices=new(t_Device[40]);
	if(Devices){
		for(uint8_t id=0; id<40; id++) {
			Devices[id].id = id;
			Devices[id].callbackHandler = 0;
			Devices[id].deviceHandler = 0;
			Devices[id].deviceType = HW_TYPE_NOP;
			Devices[id].pin = GPIO_NOTDEFINED;
		}
	}

	hw_recall=(Hardware **)malloc(sizeof(unsigned int) * Max_numberOfHWRecall);

	tmp_result = esp_event_loop_create(&event_args, &event_loop_handler);
	if (tmp_result != ESP_OK){
		ESP_LOGE(SYSTEM_TAG,"error! %s", esp_err_to_name(tmp_result)); // @suppress("Invalid arguments")
		ESP_ERROR_CHECK(tmp_result);
	}else{
		tmp_result = esp_event_handler_instance_register_with(event_loop_handler, TASK_EVENTS, TASK_SYSTEM_EVENT,\
						event_handler, event_loop_handler, NULL); // @suppress("Invalid arguments")
		if (tmp_result != ESP_OK){
			ESP_LOGE(SYSTEM_TAG,"error! %s", esp_err_to_name(tmp_result)); // @suppress("Invalid arguments")
			ESP_ERROR_CHECK(tmp_result);
		}
	}
// */
}

void Hardware::init(){
//	uint32_t numberOfHandlerClasses = 0;
//	uint32_t maxNumberOfHandlerClasses = 2147483640;
}

Hardware::~Hardware() {
	// TODO Auto-generated destructor stub
//	if(Devices)delete(Devices);
}

/***************************************************************************************
** Function name:			registerType
** Description :			Function calling by device handler (driver) to initiate itself
** 							and initiates driver->loop() function. loop()_ function mast work
** 							with device itself and call registered handlers if device's event occure.
** 							Same time handler(...) function mast be implemented to work with
** 							any messages from application to device-driver.
** Args:
**			arg1 - value
**
** Return:	id, or 0 if error
***************************************************************************************/
uint32_t Hardware::registerType(t_regHW* reg) {
	if (numberOfHandlerClasses >= maxNumberOfHandlerClasses) {
		ESP_LOGE(SYSTEM_TAG,"registerType: Can't add callback handler. Maximum reached!"); // @suppress("Invalid arguments")
		return 0;
	}
	if(heap_caps_get_free_size(MALLOC_CAP_INTERNAL) < (sizeof(t_regHW*) * (numberOfHandlerClasses + 1))){ // check if enough free space
		ESP_LOGE(SYSTEM_TAG,"registerType: RAM is out! %u < %u (%u / %u) ",
				heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
				(sizeof(t_regHW*) * (numberOfHandlerClasses + 1)),
				sizeof(t_regHW*),
				(numberOfHandlerClasses + 1)); // @suppress("Invalid arguments")
		return 0;
	}
/*Serial.printf("registerType: %u < %u (%u / %u) ",
heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
(sizeof(t_regHW*) * (numberOfHandlerClasses + 1)),
sizeof(t_regHW*),
(numberOfHandlerClasses + 1));*/
	if (!numberOfHandlerClasses){
//Serial.printf(" handler not registered!\n");
		HandlerClasses = (t_regHW**)malloc(sizeof(t_regHW*));
	}else{
//Serial.printf(" handler registered!\n");
		HandlerClasses = (t_regHW**)realloc(HandlerClasses, sizeof(t_regHW*) * (numberOfHandlerClasses + 1));
	}
//Serial.printf("allocated \n");
	HandlerClasses[numberOfHandlerClasses] = reg;
//Serial.printf("moved \n");

	esp_event_loop_handle_t loop_task;
//Serial.printf("loop_task \n");

	esp_event_loop_create(&reg->eventLoopArgs, &loop_task);// ToDo: make device-driver-loop() function &((Hardware(*)(void))((Hardware*) reg->handleClass)->loop));
//Serial.printf("loop_task created\n");

	esp_event_handler_instance_register_with(loop_task, reg->eventLoopArgs.task_name, DEVICE_HARDWARE_EVENT_DELETE, (esp_event_handler_t)(((Hardware *)reg->handleClass)->handler), reg->eventHandlerArg, NULL);
//Serial.printf("loop_task registered\n");

	BaseType_t tmp_result = xTaskCreate(reg->fn_loop, reg->eventLoopArgs.task_name, reg->eventLoopArgs.task_stack_size, NULL, uxTaskPriorityGet(NULL), NULL); // @suppress("Invalid arguments") // @suppress("Field cannot be resolved")
	//BaseType_t tmp_result = xTaskCreatePinnedToCore((((Hardware*)reg->handleClass)->loop()), reg->eventLoopArgs.task_name, reg->eventLoopArgs.task_stack_size, NULL, uxTaskPriorityGet(NULL), NULL, 0); // last 0 (mean core0) may be 1 (for core1) or tskNO_AFFINITY (any core)
//Serial.printf("loop_task started reg=%u, handler[]=%u\n", (unsigned int) reg, (unsigned int) HandlerClasses[0]);

	if (tmp_result == pdPASS){
		numberOfHandlerClasses++;
		uint32_t tmp_id = 0;
		uint tmp_min = 1;
//Serial.printf("running loop \n");
		while((tmp_id <= maxNumberOfHandlerClasses) && (tmp_id < numberOfHandlerClasses)) {
			tmp_id++;
//Serial.printf("running loop %u / %u \n", tmp_id, numberOfHandlerClasses);
			if(HandlerClasses[(tmp_id-1)]->id == tmp_min) {tmp_min++; tmp_id = 0;}
		}
//Serial.printf("ending loop \n");
		reg->id = tmp_min;
		ESP_LOGI(SYSTEM_TAG,"registerType: Task created %s", reg->eventLoopArgs.task_name); // @suppress("Invalid arguments") // @suppress("Field cannot be resolved")
		return reg->id; // task created
	}

	ESP_LOGE(SYSTEM_TAG,"registerType: Can't register device '%s'! %s", reg->eventLoopArgs.task_name, esp_err_to_name(tmp_result)); // @suppress("Invalid arguments") // @suppress("Field cannot be resolved")
	return 0;


}




void Hardware::handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
	;
}


void Hardware::loop(){
	while(true){
		Serial.printf("HWdriver running.\n"); // %u\n", numberOfHandlerClasses);
		vTaskDelay((unsigned int) (1000/portTICK_PERIOD_MS));
	}
}
/*
bool Hardware::add(uint8_t  type, void* data,Hardware* hw_call, esp_event_loop_args_t* return_elArgs){
	if (numberOfHWRecall >= maxNumberOfHandlerClasses) {
		ESP_LOGE(SYSTEM_TAG,"Can't add callback handler. Maximum reached!"); // @suppress("Invalid arguments")
		return 0;
	}
	if (hw_call == 0){
		ESP_LOGE(SYSTEM_TAG,"Callback handler is null!"); // @suppress("Invalid arguments")
		return 0;
	}
	hw_recall[numberOfHWRecall] = hw_call;
	numberOfHWRecall++;


	return 1;
}
*/











/***************************************************************************************
** Function name:			add
** Description :			initializes new device (pin)
** Args:
**			arg1 - value
**
** Return:	id, or 0 if error
***************************************************************************************/
/*uint8_t Hardware::addDI(uint8_t pin, void callbackHandler(t_Data*)) {
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
uint8_t Hardware::addAI(uint8_t pin, void callbackHandler(t_Data*)) {
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
/***************************************************************************************
** Function name:			run
** Description :			loop-task for handle work of Bus
***************************************************************************************/
/*void Hardware::run() {

	t_Data * rvalue;
	int rv;
	// Check every initiate pin

	for (uint8_t id = 0; id < 40; id++){
		if(Devices[id].pin != GPIO_NOTDEFINED) {
			// check pin type
			if(Devices[id].deviceType == HW_TYPE_DI) {
				// check input
				rvalue=new(t_Data);
				rvalue->data = malloc(sizeof(int));
				rvalue->deviceType=Devices[id].deviceType;
				rvalue->pin=Devices[id].pin;
				rv = digitalRead(Devices[id].pin);
				memcpy(rvalue->data,(const void *)&rv,sizeof(int));
				(*Devices[id].callbackHandler)( rvalue);
				free(rvalue->data);
				delete(rvalue);
			}
			if(Devices[id].deviceType == HW_TYPE_AI) {
				// check input
				rvalue=new(t_Data);
				rvalue->data = malloc(sizeof(int));
				rvalue->deviceType=Devices[id].deviceType;
				rvalue->pin=Devices[id].pin;
				rv = analogRead(Devices[id].pin);
				memcpy(rvalue->data,(const void *)&rv,sizeof(int));
				(*Devices[id].callbackHandler)( rvalue);
				free(rvalue->data);
				delete(rvalue);
			}

		}

	}


	delay(5);
}
// Default function to work with devices
uint8_t Hardware::msg(uint8_t deviceID, uint8_t msg, void * payload){
	return MSG_SYS_NOP; // Just
}

void Hardware::event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data){

	if(event_handler_arg == event_args){
		// system events
		Serial.printf("[System event handler] running event");
	}

}
*/
} /* namespace Hardware */
} /* namespace Device */

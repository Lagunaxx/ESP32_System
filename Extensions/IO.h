/*
 * IO.h
 *
 *  Created on: 25 апр. 2022 г.
 *      Author: producer
 */

#ifndef COMPONENTS_ESP32_SYSTEM_EXTENSIONS_IO_H_
#define COMPONENTS_ESP32_SYSTEM_EXTENSIONS_IO_H_

#include "ESP32_System.h"

namespace Device {
namespace Hardware {

class class_IO{//: private Hardware{
public:
	class_IO();
	virtual ~class_IO();

	static void IO_loop(void * args); // main loop of IO

	uint8_t addDI(uint8_t pin, void callbackHandler(t_Data*));	// Add new DI device
	uint8_t addAI(uint8_t pin, void callbackHandler(t_Data*));	// Add new AI device

private:

	t_Device *Devices;

	unsigned int delay; // Delay between reading data

};

} /* namespace Hardware */
} /* namespace Device */

#endif /* COMPONENTS_ESP32_SYSTEM_EXTENSIONS_IO_H_ */

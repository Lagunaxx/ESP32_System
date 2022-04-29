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

	static void _loop(void *); // main loop of IO

};

} /* namespace Hardware */
} /* namespace Device */

#endif /* COMPONENTS_ESP32_SYSTEM_EXTENSIONS_IO_H_ */

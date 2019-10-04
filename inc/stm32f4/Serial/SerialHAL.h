/*
 * STM32 HAL implementation of Serial library.
 *
 * ***** IMPORTANT: *****
 * You need to enable the USART interrupt in CubeMX, under NVIC Settings for the USART.  Without this, Tx will
 * work but you will never receive anything.
 *
 * Note that this library currently uses blocking mode for writing, and non blocking (1 byte at a time) interrupt mode
 * for reading.  Some people have reported issues with reading one byte at a time when using a fast baud rate and with
 * a busy CPU; if this is encountered, we may need to look at DMA mode to a ring buffer or something.
 */

#ifndef SERIAL_HAL_H
#define SERIAL_HAL_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"

#include <Stream.h>
#include <ArrayStream.h>

#define SERIAL_HAL_MAX_INSTANCES			8

namespace digitalcave {

	class SerialHAL : public Stream {
		private:
			ArrayStream rxBuffer;
			uint8_t incomingByte;
			UART_HandleTypeDef* huart;

		public:
			//Keep track of instantiated instances, to delegate isr() and error() calls
			static SerialHAL* instances[SERIAL_HAL_MAX_INSTANCES];

			//Initialize specifying baud rate and all other optional parameters
			SerialHAL(UART_HandleTypeDef* huart, uint8_t bufferSize);

			//Return the pointer to the UART; can be used to verify that the ISR only fires on this serial port
			UART_HandleTypeDef* getHandleTypeDef() { return huart; }

			// Implementation of virtual functions declared in superclass
			uint8_t read(uint8_t *b);
			uint8_t write(uint8_t data);

			//Notify serial library that there is a byte ready for reading.  This MUST be called by the serial read ISR.
			void isr();
			void error();

			using Stream::read; // Allow other overloaded functions from superclass to show up in subclass.
			using Stream::write; // Allow other overloaded functions from superclass to show up in subclass.
	};
}

#endif

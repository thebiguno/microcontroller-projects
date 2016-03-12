/*
 * STM32 HAL implementation of Serial library.
 * 
 * ***** IMPORTANT: *****
 * You need to enable the USART interrupt in CubeMX, under NVIC Settings for the USART.  Without this, Tx will
 * work but you will never receive anything.
 * You also need to put an ISR callback into your own code somewhere to handle incoming characters and pass them to the 
 * serial object.  To keep the library code separate and clean, we can't put this into the library itself.
 * 
 *		void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
 * 			if (serial){
 *				serial->isr();
 * 			}
 *		}
 * 
 * Note that the serial object must not be initialized until after MX_USARTx_UART_Init is called in the main code.
 * The best way to handle this is to define a pointer:
 *		SerialHAL* serial;
 * as a global variable, but don't initialize it until later: 
 *		SerialHAL s(&huart2, 64);
 *		serial = &s;
 * See sample serial_echo for an example of this.
 * 
 * Note that this library currently uses blocking mode for writing, and non blocking (1 byte at a time) interrupt mode
 * for reading.  Some people have reported issues with reading one byte at a time when using a fast baud rate and with
 * a busy CPU; if this is encountered, we may need to look at DMA mode to a ring buffer or something.
 */

#ifndef SERIAL_HAL_H
#define SERIAL_HAL_H

#include "stm32f4xx_hal.h"

#include <Stream.h>
#include <ArrayStream.h>

namespace digitalcave {

	class SerialHAL : public Stream {
		private:
			ArrayStream rxBuffer;
			uint8_t incomingByte;
			UART_HandleTypeDef* huart;
			
		public:
			//Initialize specifying baud rate and all other optional parameters
			SerialHAL(UART_HandleTypeDef* huart, uint8_t bufferSize);
			
			// Implementation of virtual functions declared in superclass
			uint8_t read(uint8_t *b);
			uint8_t write(uint8_t data);
			
			//Notify serial library that there is a byte ready for reading.  This MUST be called by the serial read ISR.
			void isr();
			
			using Stream::read; // Allow other overloaded functions from superclass to show up in subclass.
			using Stream::write; // Allow other overloaded functions from superclass to show up in subclass.
	};
}

#endif
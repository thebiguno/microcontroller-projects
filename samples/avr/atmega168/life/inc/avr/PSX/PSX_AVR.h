/*
 * AVR implementation of PSX library.
 */

#ifndef PSX_AVR_H
#define PSX_AVR_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include <PSX.h>

//Half a clock cycle; 10µs seems to be about right
#define CTRL_CLK        15


namespace digitalcave {

	class PSX_AVR : public PSX {
		private:
			//Define ports used
			volatile uint8_t *data_port;
			volatile uint8_t *clock_port;
			volatile uint8_t *command_port;
			volatile uint8_t *attention_port;

			//Define the input
			volatile uint8_t *data_in;

			//Define ddr used
			volatile uint8_t *data_ddr;
			volatile uint8_t *clock_ddr;
			volatile uint8_t *command_ddr;
			volatile uint8_t *attention_ddr;

			//Define pins to use
			uint8_t clock_pin;
			uint8_t command_pin;
			uint8_t attention_pin;
			uint8_t data_pin;
			
			// Implementation of virtual functions declared in superclass
			virtual void sendCommand(uint8_t send_data[], uint8_t size);
			
		public:
			//Initialize specifying baud rate and all other optional parameters
			PSX_AVR(volatile uint8_t *dataPort, uint8_t dataPin,				//MISO (Brown)
						volatile uint8_t *commandPort, uint8_t commandPin,		//MOSI (Orange)
						volatile uint8_t *clockPort, uint8_t clockPin,			//SCK (Blue)
						volatile uint8_t *attentionPort, uint8_t attentionPin);	//CS (Yellow)
	};
}

#endif
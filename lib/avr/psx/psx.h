#ifndef PSX_lib_h
#define PSX_lib_h

#include "AbstractPsx.h"

namespace digitalcave {
	class Psx : public AbstractPsx {
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
		
		uint8_t shift(uint8_t b);
		void sendCommand(uint8_t send_data[], uint8_t size);
		
	public:
		/* Connect controller wires as follows:
			Brown:  Data
			Blue:   Clock
			Orange: Command
			Yellow: Attention
		*/
		Psx(volatile uint8_t *data_port, uint8_t data_pin,
			volatile uint8_t *clock_port, uint8_t clock_pin,
			volatile uint8_t *command_port, uint8_t command_pin,
			volatile uint8_t *attention_port, uint8_t attention_pin);
		~Psx();
	};
}

#endif

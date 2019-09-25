#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <Serial/SerialUSB.h>
#include <Serial/SerialAVR.h>
#include <DFPlayerMini.h>


using namespace digitalcave;

SerialUSB serialUSB;
SerialAVR serialAVR(9600, 8, 0, 1, 1);		//Serial Port 1 is the hardware serial port

int main (void){
	//Do setup here
	DFPlayerMini dfplayer(&serialAVR);
	dfplayer.sendCommand(DFPLAYER_COMMAND_VOL_SET, 15);
	dfplayer.sendCommand(DFPLAYER_COMMAND_TRACK, 0);

	char buffer[64];
	uint8_t* response;

	//Main program loop
	while (1){
		if (dfplayer.poll()){
			response = dfplayer.getResponse();
			serialUSB.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "Poll: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\r", response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7], response[8], response[9]));
		}

		if (dfplayer.sendCommand(DFPLAYER_COMMAND_GET_SW_VER)){
			response = dfplayer.getResponse();
			serialUSB.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "Response: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\r", response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7], response[8], response[9]));
		}

		if (dfplayer.sendCommand(DFPLAYER_COMMAND_TRACK, 0)){
			response = dfplayer.getResponse();
			serialUSB.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "T0: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\r", response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7], response[8], response[9]));
		}

		_delay_ms(5000);

		if (dfplayer.sendCommand(DFPLAYER_COMMAND_TRACK, 1)){
			response = dfplayer.getResponse();
			serialUSB.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "T1: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\r", response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7], response[8], response[9]));
		}

		_delay_ms(5000);

		if (dfplayer.sendCommand(DFPLAYER_COMMAND_TRACK, 2)){
			response = dfplayer.getResponse();
			serialUSB.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "T2: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\r", response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7], response[8], response[9]));
		}

		_delay_ms(5000);

		if (dfplayer.sendCommand(DFPLAYER_COMMAND_TRACK, 3)){
			response = dfplayer.getResponse();
			serialUSB.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "T3: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\r", response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7], response[8], response[9]));
		}

		_delay_ms(5000);
	}
}

ISR(USART1_RX_vect){
	serialAVR.isr();
}

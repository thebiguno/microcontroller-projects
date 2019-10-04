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
	delay_ms(2000);

	//Do setup here
	DFPlayerMini dfplayer(&serialAVR);
	dfplayer.sendCommand(DFPLAYER_COMMAND_VOL_SET, 15);
//	dfplayer.sendCommand(DFPLAYER_COMMAND_TRACK, 0);

	char buffer[64];

	uint8_t i = 1;

	//Main program loop
	while (1){
		serialUSB.write("Querying tracks...\n\r");
		dfplayer.sendCommand(DFPLAYER_COMMAND_GET_FILE_COUNT, 0);
		while(dfplayer.poll());

		delay_ms(5000);

		// serialUSB.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "Requesting track %d\n\r", i));
		dfplayer.sendCommand(DFPLAYER_COMMAND_VOL_SET, 10);
		dfplayer.sendCommand(DFPLAYER_COMMAND_FOLDER_SET, 0x0100 + 14);
		//
		// while(dfplayer.poll());
		//
		// while(1){
		// 	_delay_ms(5000);
		// 	if (dfplayer.poll()){
		// 		break;
		// 	}
		// }
		//
		// dfplayer.sendCommand(DFPLAYER_COMMAND_GET_STATUS, 0x00);
		//
		// i++;
		// if (i > 12){
		// 	i = 1;
		// }
	}
}

ISR(USART1_RX_vect){
	serialAVR.isr();
}

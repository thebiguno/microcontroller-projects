#include "DFPlayerMini.h"
#include <SerialUSB.h>

using namespace digitalcave;

extern SerialUSB serialUSB;

DFPlayerMini::DFPlayerMini(Stream* serial) :
	serial(serial)
{
	//Constant data bytes in request
	request[0] = 0x7E;	//Start byte
	request[1] = 0xFF;	//Version info
	request[2] = 0x06;	//Length (constant)
	request[9] = 0xEF;	//End byte

	delay_ms(1000);
	init();
}

void DFPlayerMini::init(){
	sendCommand(DFPLAYER_COMMAND_RESET);			//Reset the module

	//Wait until the module comes online, for a max of 5 seconds (200 * 25ms)
	for (uint8_t i = 0; i < 200; i++){
		sendCommand(DFPLAYER_COMMAND_GET_STATUS);
		if (poll()){
			break;
		}
		delay_ms(25);
	}

	//Finish initializing the module
	sendCommand(DFPLAYER_COMMAND_STANDBY_OFF);		//Turn off standby
	sendCommand(DFPLAYER_COMMAND_VOL_SET, 0);		//Volume 0
	sendCommand(DFPLAYER_COMMAND_EQ_SET, 0);		//Normal EQ
	sendCommand(DFPLAYER_COMMAND_MODE_SET, 0);		//Repeat
	sendCommand(DFPLAYER_COMMAND_SOURCE_SET, 1);	//TF Source (micro SD card)
}

uint8_t DFPlayerMini::sendCommand(uint8_t command, uint16_t arg){
	uint8_t feedback = 0x01;	//(command >= 0x3C ? 0x01 : 0x00);		//Commands over 0x3C have a return code

	request[3] = command;
	request[4] = feedback;	//Feedback requested
	request[5] = (arg >> 8) & 0xFF;	//High byte
	request[6] = arg & 0xFF;			//Low byte
	uint16_t checksum = 0 - (request[1] + request[2] + request[3] + request[4] + request[5] + request[6]);
	request[7] = (checksum >> 8) & 0xFF;
	request[8] = checksum & 0xFF;
	//serialUSB.write("writing command... ");
	serial->write(request, 10);
	//serialUSB.write("done\n\r");

	delay_ms(10);	//Wait 10ms to be sure command is completed

	if (feedback){
		delay_ms(20);	//Wait a bit longer if we are expecting a reply - at 9600 baud one message takes about 10 ms to send, and another 10 to recieve.  Give it an extra 10 for good measure.
		if (poll()){
			return 1;
		}
	}

	return 0;
}

uint8_t DFPlayerMini::poll(){
	uint8_t buffer[10];
	buffer[0] = 0x00;
	//Discard bytes until we find the start byte
	while (serial->read(buffer)){
		if (buffer[0] == 0x7E){
			break;
		}
	}

	if (buffer[0] != 0x7E){
		return 0;	//Nothing in the queue
	}

	//Once we started receiving a command, we can wait up to 5ms for each byte to recieve the rest of it.
	for (uint8_t i = 1; i < 10; i++){
		if (!serial->read(&buffer[i])){
			delay_ms(5);
			if (!serial->read(&buffer[i])){
				return 0;
			}
		}
	}

	//Copy buffer to the response
	for (uint8_t i = 0; i < 10; i++){
		response[i] = buffer[i];
	}
	return 1;
}

uint8_t* DFPlayerMini::getResponse(){
	return response;
}

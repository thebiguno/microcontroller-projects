#include "DFPlayerMini.h"

#ifdef DEBUG
#include <SerialUSB.h>
#include <stdio.h>
#endif

using namespace digitalcave;

#ifdef DEBUG
extern SerialUSB serialUSB;
#endif

DFPlayerMini::DFPlayerMini(Stream* serial) :
	serial(serial)
{
	//Constant data bytes in request
	request[0] = 0x7E;	//Start byte
	request[1] = 0xFF;	//Version info
	request[2] = 0x06;	//Length (constant)
	request[9] = 0xEF;	//End byte
	delay_ms(500);
	//init();
}

void DFPlayerMini::init(){
	//Wait until the module comes online, for a max of 5 seconds (200 * 25ms)
	for (uint8_t i = 0; i < 200; i++){
		if (sendCommand(DFPLAYER_COMMAND_GET_INIT_PARM)){
			break;
		}
		delay_ms(25);
	}

	//Finish initializing the module
	sendCommand(DFPLAYER_COMMAND_STANDBY_OFF);		//Turn off standby
	sendCommand(DFPLAYER_COMMAND_VOL_SET, 0);		//Volume 0
	sendCommand(DFPLAYER_COMMAND_EQ_SET, 0);		//Normal EQ
	sendCommand(DFPLAYER_COMMAND_MODE_SET, 0);		//Repeat
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

#ifdef DEBUG
	char temp[64];
	serialUSB.write((uint8_t*) temp, (uint16_t) snprintf(temp, sizeof(temp), "Request: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\r", request[0], request[1], request[2], request[3], request[4], request[5], request[6], request[7], request[8], request[9]));
#endif

	delay_ms(20);	//Wait a bit longer if we are expecting a reply - at 9600 baud one message takes about 10 ms to send, and another 10 to recieve.  Give it an extra 10 for good measure.
	if (poll()){
		if (response[3] == 0x40){
#ifdef DEBUG
			serialUSB.write("Resending command...\n\r");
#endif
			sendCommand(command, arg);		//0x40 indicates the command should be re-sent
		}
		else if (response[3] == 0x41){
			return 1;
		}
	}

	return 0;
}

uint8_t* DFPlayerMini::poll(){
	uint8_t buffer[10];
	buffer[0] = 0x00;
	//Discard bytes until we find the start byte
	while (serial->read(buffer)){
		if (buffer[0] == 0x7E){
			break;
		}
	}

	if (buffer[0] != 0x7E){
		return NULL;	//Nothing in the queue
	}

	//Once we started receiving a command, we can wait up to 5ms for each byte to recieve the rest of it.
	for (uint8_t i = 1; i < 10; i++){
		if (!serial->read(&buffer[i])){
			delay_ms(5);
			if (!serial->read(&buffer[i])){
				return NULL;	//Timed out
			}
		}
	}

	//Copy buffer to the response
	for (uint8_t i = 0; i < 10; i++){
		response[i] = buffer[i];
	}
#ifdef DEBUG
	char temp[64];
	serialUSB.write((uint8_t*) temp, (uint16_t) snprintf(temp, sizeof(temp), "Response: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\r", response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7], response[8], response[9]));
#endif
	return response;
}

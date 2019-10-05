#include "DFPlayerMini.h"

using namespace digitalcave;

static Stream* _serial = NULL;
static ArrayStream buffer(64);

static uint8_t request[10];
static uint8_t response[10];


void dfplayermini_init(Stream* serial){
	_serial = serial;

	//Empty the serial buffer
	uint8_t b;
	while(_serial->read(&b));

	//Constant data bytes in request
	request[0] = 0x7E;	//Start byte
	request[1] = 0xFF;	//Version info
	request[2] = 0x06;	//Length (constant)
	request[9] = 0xEF;	//End byte

	//Wait until the module comes online, for a max of 2 seconds (20 * 50ms)
	for (uint8_t i = 0; i < 40; i++){
		//We can either wait for it to send a 0x3F init command...
		if (dfplayermini_poll() && response[3] == DFPLAYER_COMMAND_GET_INIT_PARM){
			break;
		}
		//.. or we can ask for it and get a 0x41 success command back
		else if (dfplayermini_send_command(DFPLAYER_COMMAND_GET_INIT_PARM)){
			break;
		}
		delay_ms(50);
	}

	//Finish initializing the module
	dfplayermini_send_command(DFPLAYER_COMMAND_STANDBY_OFF);		//Turn off standby
	dfplayermini_send_command(DFPLAYER_COMMAND_VOL_SET, 0);		//Volume 0
	dfplayermini_send_command(DFPLAYER_COMMAND_EQ_SET, 0);		//Normal EQ
	dfplayermini_send_command(DFPLAYER_COMMAND_MODE_SET, 0);		//Repeat
}

uint8_t dfplayermini_send_command(uint8_t command, uint16_t arg){
	uint8_t feedback = 0x01;	//(command >= 0x3C ? 0x01 : 0x00);		//Commands over 0x3C have a return code

	request[3] = command;
	request[4] = feedback;	//Feedback requested
	request[5] = (arg >> 8) & 0xFF;	//High byte
	request[6] = arg & 0xFF;			//Low byte
	uint16_t checksum = 0 - (request[1] + request[2] + request[3] + request[4] + request[5] + request[6]);
	request[7] = (checksum >> 8) & 0xFF;
	request[8] = checksum & 0xFF;
	_serial->write(request, 10);

	delay_ms(20);	//Wait a bit longer - at 9600 baud one message takes about 10 ms to send, and another 10 to recieve.  Give it an extra 10 for good measure.
	while (dfplayermini_poll() != NULL){
		if (response[3] == 0x40){
			dfplayermini_send_command(command, arg);		//0x40 indicates the command should be re-sent
		}
		else if (response[3] == 0x41){		//0x41 is success
			return 1;
		}
	}

	return 0;
}

uint8_t* dfplayermini_poll(){
	uint8_t b = 0;
	//Read anything from the serial port into a buffer
	while (_serial->read(&b)){
		buffer.write(b);
	}

	//Discard bytes until we have less than 2 messages, and find a start byte
	while ((buffer.peek(&b) && b != 0x7E) || buffer.size() > 20){
		buffer.read(&b);
	}

	if (buffer.size() > 10){
		//Copy 10 bytes of buffer to response
		for (uint8_t i = 0; i < 10; i++){
			buffer.read(&b);
			response[i] = b;
		}
		return response;
	}
	else {
		return NULL;	//Not a full message in the queue
	}
}

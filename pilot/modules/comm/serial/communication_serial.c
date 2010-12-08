#include "../communication.h"
#include "../../../lib/serial/serial.h"

void comm_init()
{
	serial_init(57600, 8, 0, 1);
}

void comm_write(uint8_t b) {
	serial_write_c((char) b);
}

uint8_t comm_available() {
	return serial_available();
}

uint8_t comm_read(uint8_t *b) {
	return serial_read_c((char *) b);
}


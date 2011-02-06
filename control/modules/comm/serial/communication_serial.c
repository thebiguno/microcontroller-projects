#include "../communication.h"
#include "../../../lib/serial/serial.h"
#include "../../../lib/serial1/serial1.h"

void comm_init(){
	serial_init_b(57600);
	serial1_init_b(57600);
}

void comm_pilot_write(uint8_t b) {
	serial_write_c((char) b);
}
uint8_t comm_pilot_available() {
	return serial_available();
}
uint8_t comm_pilot_read(uint8_t *b) {
	return serial_read_c((char *) b);
}

void comm_pc_write(uint8_t b) {
	serial1_write_c((char) b);
}
uint8_t comm_pc_available() {
	return serial1_available();
}
uint8_t comm_pc_read(uint8_t *b) {
	return serial1_read_c((char *) b);
}

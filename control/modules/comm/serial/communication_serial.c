#include "../communication.h"
#include "../../../lib/serial/serial.h"

void comm_init(){
	serial_init(9600, 8, 0, 1);
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
	// serial_n_write_c(1, (char) b);
}
uint8_t comm_pc_available() {
	// return serial_n_write_c(1);
	return 0;
}
uint8_t comm_pc_read(uint8_t *b) {
	// return serial_n_read((char *) b);
	return 0;
}

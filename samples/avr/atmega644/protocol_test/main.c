/*
 * Sample skeleton source file.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>
#include "protocol.h"
#include "lib/serial/serial.h"

int main (void){
	serial_init_b(57600);
	
	_delay_ms(3000);
		
	uint8_t buf[36];

	int i;

	// 00000000, 80000000, 3F800000, BF800000, 7F800000, FF800000, 7FC00000, 00000001, 80000001
	double x1[9] = { 0.0, -0.0, 1.0, -1.0, INFINITY, -INFINITY, NAN, 1.4012985e-45, -1.4012985e-45 };
	for (i = 0; i < 9; i++) {
		protocol_double_to_bytes(x1[i], buf, i * 4);
	}
	protocol_send_message('F', buf, 36);
	
	// C0490FDB, C032B8C2, C01C61AA, C0060A92, BFDF66F3, BFB2B8C2, BF860A92, BF32B8C2, BEB2B8C2
	double x2[9] = { -3.1415927, -2.7925267, -2.4434609, -2.0943952, -1.7453293, -1.3962634, -1.0471976, -6.9813168e-1, -3.4906584e-1 };
	for (i = 0; i < 9; i++) {
		protocol_double_to_bytes(x2[i], buf, i * 4);
	}
	protocol_send_message('F', buf, 36);
	
	// 40490FDB, 4032B8C2, 401C61AA, 40060A92, 3FDF66F3, 3FB2B8C2, 3F860A92, 3F32B8C2, 3EB2B8C2
	double x3[9] = { 3.1415927, 2.7925267, 2.4434609, 2.0943952, 1.7453293, 1.3962634, 1.0471976, 6.9813168e-1, 3.4906584e-1 };
	for (i = 0; i < 9; i++) {
		protocol_double_to_bytes(x3[i], buf, i * 4);
	}
	protocol_send_message('F', buf, 36);

	// 3DCCCCCD, 3E99999A, 3F333333, 3C23D70A, 3CF5C28F, 3D8F5C29, 3A83126F, 3B449BA6, 3BE56042, 
	double x4[9] = { 0.1, 0.3, 0.7, 0.01, 0.03, 0.07, 0.001, 0.003, 0.007, };
	for (i = 0; i < 9; i++) {
		protocol_double_to_bytes(x4[i], buf, i * 4);
	}
	protocol_send_message('F', buf, 36);
	
	while(1) {
	}	
}


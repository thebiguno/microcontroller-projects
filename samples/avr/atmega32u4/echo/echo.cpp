#include <stdint.h>
#include <SerialAVR.h>

using namespace digitalcave;

int main (void) {
	
	SerialAVR serial(115200);
	uint8_t b;
	while (1) {
		while (!serial.read(&b));
		serial.write(b);
	}
	
}

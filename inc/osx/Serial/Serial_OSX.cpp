#include "Serial_OSX.h"
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

using namespace digitalcave;

Serial_OSX::Serial_OSX(char* device, uint32_t baud, uint8_t parity, uint8_t stopBits) {
	
	int fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1) {
		// hmm
	}

	// configure the device
	struct termios options;
	tcgetattr(fd, &options);
	cfsetispeed(&options, baud);
	cfsetospeed(&options, baud);

	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_cflag |= (CLOCAL | CREAD);
	tcsetattr(fd, TCSANOW, &options);
}

uint8_t Serial_OSX::read(uint8_t* b) {
	uint8_t buffer[1];
	int8_t sz = read(fd, &b, 1);
	if (sz > 0) return 1;
	else return 0;
}

uint8_t Serial_OSX::write(uint8_t b) {
	int8_t sz = write(fd, &b, 1);
	if (sz > 0) return 1;
	else return 0;
}

void close() {
	close(fd);
}
//Implementation of AVR I2C object.  This is a thin wrapper around the lower level twi library.
#include "I2CAVR.h"

using namespace digitalcave;

I2CAVR::I2CAVR() {
	twi_init();
}

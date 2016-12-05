/*
 * AVR implementation of I2C library.  This is a thin wrapper around the lower level twi library, and as such,
 * should really be a singleton.  Making multiple instances of this object still accesses the same underlying hardware.
 */

#ifndef I2C_AVR_H
#define I2C_AVR_H

#include <twi/twi.h>
#include <I2C/I2C.h>

namespace digitalcave {

	class I2CAVR : public I2C {
		public:
			//Initialize thin wrapper object
			I2CAVR();

			// Implementation of virtual functions declared in superclass
			void write(uint8_t address, I2CMessage* m) { twi_write_to(address, m->getData(), m->getLength(), I2C_BLOCK, I2C_STOP); }
			void read(uint8_t address, I2CMessage* m) { twi_read_from(address, m->getData(), m->getLength(), I2C_STOP); }

			using I2C::read; // Allow other overloaded functions from superclass to show up in subclass.
			using I2C::write; // Allow other overloaded functions from superclass to show up in subclass.
	};
}

#endif

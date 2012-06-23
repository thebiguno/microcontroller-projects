#ifndef FM_H
#define FM_H

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "../i2c/i2c_master.h"
#include "../serial/serial.h"

#define SI4703_ADDRESS	0x10

//Define the register names
#define DEVICEID 0x00
#define CHIPID  0x01
#define POWERCFG  0x02
#define CHANNEL  0x03
#define SYSCONFIG1  0x04
#define SYSCONFIG2  0x05
#define STATUSRSSI  0x0A
#define READCHAN  0x0B
#define RDSA  0x0C
#define RDSB  0x0D
#define RDSC  0x0E
#define RDSD  0x0F

//Register 0x02 - POWERCFG
#define SMUTE  15
#define DMUTE  14
#define SKMODE  10
#define SEEKUP  9
#define SEEK  8

//Register 0x03 - CHANNEL
#define TUNE  15

//Register 0x04 - SYSCONFIG1
#define RDS  12
#define DE  11

//Register 0x05 - SYSCONFIG2
#define SPACE1  5
#define SPACE0  4

//Register 0x0A - STATUSRSSI
#define RDSR  15
#define STC  14
#define SFBL  13
#define AFCRL  12
#define RDSS  11
#define STEREO  8

namespace digitalcave {
	
	class FM {
		private:
			// Port / pin for /RST
			volatile uint8_t *resetPort;
			uint8_t resetPin;
			
			uint16_t si4703_registers[16];
			
			void readRegisters();
			
			uint8_t updateRegisters();
			
		public:
			/*
			 * Initializes the FM tuner player, using the given port / pin for /RST.
			 */
			FM(volatile uint8_t *resetPort, uint8_t resetPin);
			int getChannel();
			uint16_t seek(uint8_t seekDirection);
			void setChannel(uint16_t channel);
			void setVolume(uint8_t volume);
	};
}

#endif
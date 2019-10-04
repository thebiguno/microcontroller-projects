/*
Library updated to comply with digitalcave libraries, removing Arduino dependencies.
Wyatt Olson, 2012-06-23

Library for Sparkfun Si4703 breakout board.
Simon Monk. 2011-09-09

This is a library wrapper and a few extras to the excellent code produced
by Nathan Seidle from Sparkfun (Beerware).

Nathan's comments......

Look for serial output at 57600bps.

The Si4703 ACKs the first uint8_t, and NACKs the 2nd uint8_t of a read.

1/18 - after much hacking, I suggest NEVER write to a register without first reading the contents of a chip.
ie, don't updateRegisters without first readRegisters.

If anyone manages to get this datasheet downloaded
http://wenku.baidu.com/view/d6f0e6ee5ef7ba0d4a733b61.html
Please let us know. It seem to be the latest version of the programming guide. It had a change on page 12 (write 0x8100 to 0x07)
that allowed me to get the chip working..

Also, if you happen to find "AN243: Using RDS/RBDS with the Si4701/03", please share. I love it when companies refer to
documents that don't exist.

1/20 - Picking up FM stations from a plane flying over Portugal! Sweet! 93.9MHz sounds a little soft for my tastes,s but
it's in Porteguese.

ToDo:
Display current status (from 0x0A) - done 1/20/11
Add RDS decoding - works, sort of
Volume Up/Down - done 1/20/11
Mute toggle - done 1/20/11
Tune Up/Down - done 1/20/11
Read current channel (0xB0) - done 1/20/11
Setup for Europe - done 1/20/11
Seek up/down - done 1/25/11

The Si4703 breakout does work with line out into a stereo or other amplifier. Be sure to test with different length 3.5mm
cables. Too short of a cable may degrade reception.
*/

#ifndef Si4703_Breakout_h
#define Si4703_Breakout_h

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "../twi/twi.h"

namespace digitalcave {

	class Si4703_Breakout{
	  public:
		Si4703_Breakout(volatile uint8_t *resetPort, uint8_t resetPin);
		void powerOn();					// call in setup
		uint16_t getChannel();
		void setChannel(uint16_t channel);  	// 3 digit channel number
		uint16_t seekUp(); 					// returns the tuned channel or 0
		uint16_t seekDown();
		void setVolume(uint16_t volume); 	// 0 to 15
		void readRDS(char* message, long timeout);
										// message should be at least 9 chars
										// result will be null terminated
										// timeout in milliseconds
	  private:
		// Port / pin for /RST
		volatile uint8_t *resetPort;
		uint8_t resetPin;

		void readRegisters();
		uint8_t updateRegisters();
		uint16_t seek(uint8_t seekDirection);
		uint16_t si4703_registers[16]; //There are 16 registers, each 16 bits large
		static const uint8_t  FAIL = 0;
		static const uint8_t  SUCCESS = 1;

		static const uint8_t SI4703_ADDRESS = 0x10;
		static const uint8_t  SEEK_DOWN = 0; //Direction used for seeking. Default is down
		static const uint8_t  SEEK_UP = 1;

		//Define the register names
		static const uint8_t  DEVICEID = 0x00;
		static const uint8_t  CHIPID = 0x01;
		static const uint8_t  POWERCFG = 0x02;
		static const uint8_t  CHANNEL = 0x03;
		static const uint8_t  SYSCONFIG1 = 0x04;
		static const uint8_t  SYSCONFIG2 = 0x05;
		static const uint8_t  STATUSRSSI = 0x0A;
		static const uint8_t  READCHAN = 0x0B;
		static const uint8_t  RDSA = 0x0C;
		static const uint8_t  RDSB = 0x0D;
		static const uint8_t  RDSC = 0x0E;
		static const uint8_t  RDSD = 0x0F;

		//Register 0x02 - POWERCFG
		static const uint8_t  SMUTE = 15;
		static const uint8_t  DMUTE = 14;
		static const uint8_t  SKMODE = 10;
		static const uint8_t  SEEKUP = 9;
		static const uint8_t  SEEK = 8;

		//Register 0x03 - CHANNEL
		static const uint8_t  TUNE = 15;

		//Register 0x04 - SYSCONFIG1
		static const uint8_t  RDS = 12;
		static const uint8_t  DE = 11;

		//Register 0x05 - SYSCONFIG2
		static const uint8_t  SPACE1 = 5;
		static const uint8_t  SPACE0 = 4;

		//Register 0x0A - STATUSRSSI
		static const uint8_t  RDSR = 15;
		static const uint8_t  STC = 14;
		static const uint8_t  SFBL = 13;
		static const uint8_t  AFCRL = 12;
		static const uint8_t  RDSS = 11;
		static const uint8_t  STEREO = 8;
	};
}
#endif

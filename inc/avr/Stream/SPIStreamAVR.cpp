#include "SPIStreamAVR.h"

using namespace digitalcave;

SPIStreamAVR::SPIStreamAVR(uint8_t clkdiv){
	//Set MOSI, SS, and SCK as outputs
	DDR_SPI |= _BV(DD_MOSI) | _BV(DD_SS) | _BV(DD_SCK);

	// Enable SPI, Set as Master
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);

	//Enable 2x flag for 2, 8, and 32 clkdiv
	switch (clkdiv){
		case 2:
		case 8:
		case 32:
			SPSR |= _BV(SPI2X);		//Enable 2x flag
			break;
		default:
			SPSR &= ~_BV(SPI2X);	//Disable 2x flag
			break;
	}

	//Determine frequency
	switch (clkdiv){
		case 2:
		case 4:
			break;	//Nothing to do here; the 2 LSB are zeros
		case 8:
		case 16:
			SPCR |= _BV(SPR0);
			break;
		case 32:
		case 64:
			SPCR |= _BV(SPR1);
			break;
		default:	//Default to 128
			SPCR |= _BV(SPR0);
			SPCR |= _BV(SPR1);
			break;
	}
}

uint8_t SPIStreamAVR::read(uint8_t *c){
	return 0;
}

uint8_t SPIStreamAVR::write(uint8_t data){
	SPDR = data;

	//Wait until transmission complete
	while(!(SPSR & _BV(SPIF) ));

	return 1;
}

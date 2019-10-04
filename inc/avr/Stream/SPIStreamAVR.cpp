#include "SPIStreamAVR.h"

using namespace digitalcave;

SPIStreamAVR::SPIStreamAVR(){
	//Set MOSI, SS, and SCK as outputs
	DDR_SPI |= _BV(DD_MOSI) | _BV(DD_SS) | _BV(DD_SCK);

	// Enable SPI, Set as Master
	// Prescaler: Fosc/16, Enable Interrupts
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1);
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

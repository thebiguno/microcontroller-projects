#include "NullSerial.h"
#include <util/delay.h>
using namespace digitalcave;

NullSerial::NullSerial(){
}

uint8_t NullSerial::read(uint8_t *c){
	return 0;
}

uint8_t NullSerial::write(uint8_t b){
	return 1;
}

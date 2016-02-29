#include "NullStream.h"

using namespace digitalcave;

NullStream::NullStream(){
}

uint8_t NullStream::read(uint8_t *c){
	return 0;
}

uint8_t NullStream::write(uint8_t b){
	return 1;
}

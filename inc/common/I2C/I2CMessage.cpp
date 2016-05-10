#include "I2CMessage.h"

using namespace digitalcave;

I2CMessage::I2CMessage(uint8_t* data, uint8_t length):
    data(data),
    length(length)
{
    ;
}

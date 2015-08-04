#ifndef DIGITALPOTUTIL_H
#define DIGITALPOTUTIL_H

#include "../State.h"

using namespace digitalcave;

/*
 * Follow on-screen instructions to calibrate the specified channel
 */
void calibrateChannel(uint8_t channel, CharDisplay display, Bounce button);

#endif
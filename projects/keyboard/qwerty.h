#ifndef QWERTY_H
#define QWERTY_H

#include <avr/pgmspace.h>
#include "hid.h"
#include "usbdrv/usbdrv.h"

void lookup_qwerty(uint8_t row, uint8_t col, uchar *key, uchar *modifier_flags);

#endif


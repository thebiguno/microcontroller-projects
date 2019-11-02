#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#define BOOTLOADER_ATMEL		0
#define BOOTLOADER_LUFA			1

#if defined(__AVR_ATmega32U4__)

#include <avr/interrupt.h>
#include <util/delay.h>

#if defined (__cplusplus)
extern "C" {
#endif

/*
 * Jump to the bootloader.
 */
void bootloader_jump(uint8_t bootloader);

#if defined (__cplusplus)
}
#endif

#endif

#endif

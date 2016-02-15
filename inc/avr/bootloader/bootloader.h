#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#if defined(__AVR_ATmega32U4__)

#include <avr/interrupt.h>
#include <util/delay.h>

#if defined (__cplusplus)
extern "C" {
#endif
	
/*
 * Jump to the bootloader.
 */
void bootloader_jump();

#if defined (__cplusplus)
}
#endif

#else
//#error Jump to Bootloader is not currently supported for this hardware.
#endif

#endif
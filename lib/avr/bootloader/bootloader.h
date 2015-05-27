#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#if defined(__AVR_ATmega32U4__)
#else
#error Jump to Bootloader is not currently supported for this hardware.
#endif

#include <avr/interrupt.h>
#include <util/delay.h>

/*
 * Jump to the bootloader.
 */
void bootloader_jump();

#endif
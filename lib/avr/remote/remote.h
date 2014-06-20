#ifndef REMOTE_H
#define REMOTE_H

#include <avr/io.h>

// all of these constants are even
// the remote may actually send odd codes depending on the odd parity with the device id
// but the library always clears the low bit and returns an even code
#define REMOTE_MENU 0x02
#define REMOTE_CENTER 0x04
#define REMOTE_RIGHT 0x06
#define REMOTE_LEFT 0x08
#define REMOTE_UP 0x0a
#define REMOTE_DOWN 0x0c

// menu + up
#define REMOTE_VIDEO_MODE 0x16
// menu + down
#define REMOTE_RESET 0x18

// these codes are just prefixes to the center code
#define REMOTE_A_CENTER 0x5c
#define REMOTE_A_PLAY 0x5e

// menu + left to pair without changing device id
// menu + center to pair whilst changing device id
// the code sent by the remote is actually 02/03 but the library ors 0xe0
#define REMOTE_PAIR 0xe2
// menu + right
// the code sent by the remote is actually 04/05 but the library ors 0xe0
// the remote does not forget it's pairing code when unpaired
#define REMOTE_UNPAIR 0xe4

/**
 * Initializes the AVR hardware to receive IR data.
 * If a paired device id is stored it should be supplied, otherwise initialize with 0x00 to accept commands from all devices.
 */
void remote_init(uint8_t deviceid);

/*
 * Returns 0 when idle.  Applications should only disable interrupts when idle.
 */
uint8_t remote_state();

void remote_reset();

/*
 * Returns the last command that has been received; or 0 if there is no new command available.
 * The command is reset when read.
 */
uint8_t remote_command();

/*
 * Returns the device id sent with the last command
 * When the command is PAIR the device id should be read and stored to eeprom or battery backed storage.
 */
uint8_t remote_deviceid();

#endif
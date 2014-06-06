#ifndef REMOTE_H
#define REMOTE_H

#include <avr/io.h>

#define REMOTE_MENU 0x02
#define REMOTE_CENTER 0x04
#define REMOTE_RIGHT 0x06
#define REMOTE_LEFT 0x08
#define REMOTE_UP 0x0a
#define REMOTE_DOWN 0x0c
#define REMOTE_VIDEO_MODE 0x16
#define REMOTE_RESET 0x18
#define REMOTE_A_CENTER 0x5c
#define REMOTE_A_PLAY 0x5e

void remote_init();

/*
 * Returns 0 when idle.  Applications should only disable interrupts when idle.
 */
uint8_t remote_state();

void remote_reset();

/*
 * Returns the last command that has been received; or 0 if there is no new command available.
 * The command is reset when read.
 */
uint8_t remote_get();

#endif
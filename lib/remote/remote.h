#ifndef REMOTE_H
#define REMOTE_H

#include <avr/io.h>

#define REMOTE_MENU = 0x03;
#define REMOTE_RIGHT = 0x07;
#define REMOTE_LEFT = 0x08;
#define REMOTE_UP = 0x0B;
#define REMOTE_DOWN = 0x0C;
#define REMOTE_CENTER = 0x5D;
#define REMOTE_PLAY = 0x5E;

void remote_init();

/*
 * Returns 0 when idle.  Applications should only disable interrupts when idle.
 */
uint8_t remote_state();

/*
 * Returns the last command that has been received; or 0 if there is no new command available.
 * The command is reset when read.
 */
uint8_t remote_get();

#endif
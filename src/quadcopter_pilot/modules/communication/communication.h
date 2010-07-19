#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "../../main.h"

/*
 * Initializes communication hardware
 */
void communication_init();

/*
 * Sends a packet to the quadcopter containing control and mode data
 */
void communication_send(control_t control, uint8_t mode);

/*
 * Reads the last received data from the quadcopter.
 */
uint16_t communication_receive();

#endif


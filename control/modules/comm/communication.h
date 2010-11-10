#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "../../main.h"

/*
 * Initializes communication hardware.
 */
void comm_init();

/*
 * Sends flight control to the pilot.
 */
void comm_tx_ctrl(control_t control, uint8_t flags);

#define FLAG_ARMED 0x01
#define FLAG_RESET_ATTITUDE 0x02
#define FLAG_CALIBRATE 0x03

#endif

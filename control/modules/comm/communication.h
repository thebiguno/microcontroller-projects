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
void comm_tx_attitude(control_t control);
void comm_tx_reset_and_calibrate();

#endif

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

#endif

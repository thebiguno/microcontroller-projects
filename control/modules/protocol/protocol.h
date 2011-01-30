#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <math.h>
#include "../../main.h"

/*
 * Poll pilot for incomming telemetry data which will be forwarded to the pc.
 */
void protocol_poll_pilot();

/*
 * Poll pc for incomming messages that should be forwarwarded to the pilot.
 */
void protocol_poll_pc();

/*
 * Sends a control message to the pilot.
 */
void protocol_send_control(control_t control);

/*
 * Sends a reset attitude message to the pilot.
 */
void protocol_send_reset_attitude();

#endif

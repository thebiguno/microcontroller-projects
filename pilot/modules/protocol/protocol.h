#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <math.h>
#include "../../main.h"

/*
 * Dispatch a message to the appropriate module.
 */
void protocol_dispatch_message(uint8_t cmd, uint8_t *bytes, uint8_t length);

/*
 * Sends a message.
 */
void protocol_send_message(uint8_t cmd, uint8_t *bytes, uint8_t length);

/*
 * Poll for new messages.
 */
void protocol_poll();

/*
 * Receives the latest flight command.
 * Returns 'A' if the command is an attitude command, 'M' if the command is a motor command, or 0x00 if there is no new command.
 */
uint8_t protocol_receive_flight_command(double values[]);

/*
 * Helper message that formats and sends telemetry.
 */
void protocol_send_telemetry(vector_t attitude, double motor[]);

/*
 * Helper message that formats and sends telemetry.
 */
void protocol_send_raw(vector_t gyro, vector_t accel);

#endif

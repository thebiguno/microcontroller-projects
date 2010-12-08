#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <math.h>
#include "../../main.h"

union udouble {
	double d;
	uint8_t u[sizeof(double)];
};

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

inline double protocol_byte_to_radian(uint8_t x) {
	double r = 0.024639942381096 * x;
	return (r > M_PI) ? 2.0 * M_PI - r : r; // convert quadrants III & IV into negative values
}
inline uint8_t protocol_radian_to_byte(float x) {
	double r = x < 0 ? 2.0 * M_PI - x : x; // convert quadrants III & IV into positive values
	return 40.584510488433314 * r;
}
inline double protocol_byte_to_percent(uint8_t x) {
	return 0.392156862745098 * x;
}
inline uint8_t protocol_percent_to_byte(float x) {
	return 2.55 * x;
}
void protocol_double_to_bytes(double value, uint8_t *buffer) {
    union udouble converter;
    converter.d = value;
    for (uint8_t i = 0; i < sizeof(double); i++) {
        buffer[i] = converter.u[i];
    }
}
double protocol_bytes_to_double(uint8_t *buffer) {
    union udouble converter;
    for (uint8_t i = 0; i < sizeof(double); i++) {
        converter.u[i] = buffer[0];
    }
    return converter.d;
}

#endif

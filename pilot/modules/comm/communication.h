#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "../../main.h"

/*
 * Initializes communication hardware.
 */
void comm_init();

/*
 * Receives an attitude control from the controller.
 * Returns the repeat count of the message, or 0 for a fresh message.
 */
uint8_t comm_rx_attitude(double *throttle, vector_t *sp, uint8_t *flags);

/*
 * Receives a motor control or an attitude control from the configuration software.
 * Returns the repeat count of the message, or 0 for a fresh message.
 * cmd has the has the bytes left/throttle, front/roll, right/pitch, back/yaw
 * flags has the bits
 *   0x0001 armed, 0x0002 reset attitude, 0x0004 calibrate gyros, 0x0008 calibrate accels, 0x0010 calibrate escs, 
 *   0x0100 RTS telemetry, 0x200 RTS pid tuning, 0x400 RTS attitute tuning, 0x0800 RTS raw sensor data
 *   0x4000 cmd mode (0=TRPY, 1=LFRB), 0x8000 write eeprom
 */
uint8_t comm_rx_control(double cmd[], uint16_t *flags);

/*
 * Receives attitude tuning values from the configuration software.
 * Returns 0 if no attitude tuning value values were available. 
 */
uint8_t comm_rx_attitude_tuning(double params[]);

/*
 * Receives pid tuning values from the configuration software.
 * Returns 0 if no PID values were available. 
 */
uint8_t comm_rx_pid_tuning(vector_t p, vector_t i, vector_t d);

/*
 * Sends telemetry to the controller.
 */
void comm_tx_telemetry(vector_t vector, double motor[], uint8_t *flags);

/*
 * Sends PID tuning values to the controller.
 */
void comm_tx_pid_tuning(vector_t p, vector_t i, vector_t d);

/*
 * Sends attitude tuning values to the controller.
 * params is 6 doubles but their meaning depends on the attitude algorithm
 *   Kalman: 0 q angle x, 1 q angle y, 2 q gyro x, 3 q gyro y, 4 r angle x, 5 r angle y
 *   Complementary: 0 x, 1 y, 2 z, 3-5 unused
 *   MHV: 0 gain, 1 beta, 2-5 unused
 *   Simple: 0 time constant, 1-5 unused
 * algorithm is 0x01 for Kalman, 0x02 Complementary, 0x04 MHV, 0x08 Simple
 */
void comm_tx_attitude_tuning(double params[], uint8_t algorithm);

#endif


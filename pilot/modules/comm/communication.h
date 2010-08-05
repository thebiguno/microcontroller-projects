#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "../../main.h"

/*
 * Initializes communication hardware.
 */
void comm_init();

/*
 * Returns > 0 when control is available.
 */
uint8_t comm_rx_ctrl_available();

/*
 * Receives flight control from the controller.
 * Calling this when comm_rx_ctrl_available() == 0 will return the same values as the previous call.
 */
void comm_rx_ctrl(control_t *control, uint8_t flags);

/*
 * Returns > 0 when pid tuning is available.
 */
uint8_t comm_rx_pid_available();

/*
 * Receives pid tuning values from the controller.
 * Calling this when comm_rx_pid_available() == 0 will return the same values as the previous call.
 */
void comm_rx_pid(vector_t *p, vector_t *i, vector_t *d);

/*
 * Sends telemetry to the controller.
 */
void comm_tx_tm(vector_t vector, double motor_a, double motor_b, double motor_c, double motor_d, uint8_t flags);

/*
 * Sends pid values to the controller.
 */
void comm_tx_pid(vector_t *p, vector_t *i, vector_t *d);


#endif


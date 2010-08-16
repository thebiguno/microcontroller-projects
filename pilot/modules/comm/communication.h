#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "../../main.h"

/*
 * Initializes communication hardware.
 */
void comm_init();

/*
 * Receives flight control from the controller.
 * Returns the repeat count of the message, or 0 for a fresh message.
 */
uint8_t comm_rx_ctrl(double *throttle, vector_t *sp, uint8_t *flags);

/*
 * Receives explicit motor controls from the configuration software.
 * Returns the repeat count of the message, or 0 for a fresh message.
 */
uint8_t comm_rx_motor(double cmd[], uint16_t flags);

/*
 * Receives attitude tuning values from the configuration software.
 * Returns 0 if no attitude tuning value values were available. 
 */
uint8_t comm_rx_params(double params[]);

/*
 * Receives pid tuning values from the configuration software.
 * Returns 0 if no PID values were available. 
 */
uint8_t comm_rx_pid(vector_t p, vector_t i, vector_t d);

/*
 * Sends telemetry to the controller.
 */
void comm_tx_tm(vector_t vector, double motor[], uint8_t *flags);

/*
 * Sends pid values to the controller.
 */
void comm_tx_pid(vector_t p, vector_t i, vector_t d);


#endif


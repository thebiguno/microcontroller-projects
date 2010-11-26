#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "../../main.h"

/*
 * Initializes communication hardware.
 */
void comm_init();
/*
 * Receives the latest command
 * command: 4 doubles
 * flags:
 */
uint8_t comm_rx_command(double command[], uint8_t *flags);

/*
 * Receives the latest tuning
 * type: 0 = PID, 1 = simple, 2 = complementary, 3 = kalman, 4 = mhv
 * payload: 9 doubles
 */
uint8_t comm_rx_tuning(uint8_t *type, double payload[]);

/*
 * Send telemetry
 */
void comm_tx_telemetry(vector_t vector, double motor[], uint8_t flags);

/*
 * Send tuning
 * type: 0 = PID, 1 = simple, 2 = complementary, 3 = kalman, 4 = mhv
 * payload: 9 doubles
 */
void comm_tx_tuning(uint8_t type, double payload[]);

#endif


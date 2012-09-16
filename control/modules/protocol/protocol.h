#ifndef PROTOCOL_H
#define PROTOCOL_H

#define PROTOCOL_COMM_TX	_BV(0)
#define PROTOCOL_COMM_RX	_BV(1)

#include <math.h>
#include "../../main.h"

void protocol_poll();

/*
 * Sends a control message to the pilot.
 */
void protocol_send_control(control_t control);
void protocol_send_kill();

/*
 * Sends a reset calibrate message to the pilot.
 */
void protocol_send_calibrate();

void protocol_send_toggle_telemetry();

void protocol_send_diag(char *);

/*
 * Sends tuning message to the pilot
 */
void protocol_send_pid_tuning(vector_t p, vector_t i, vector_t d);
void protocol_send_comp_tuning(vector_t k);
void protocol_send_kalman_tuning(vector_t qa, vector_t qg, vector_t ra);

/*
 * Read mailboxes to receive tuning messages from the pilot
 */
void protocol_get_pid_tuning(vector_t *p, vector_t *i, vector_t *d);
void protocol_get_comp_tuning(vector_t *k);
void protocol_get_kalman_tuning(vector_t *qa, vector_t *qg, vector_t *ra);

/*
 * Requests all tuning values from the pilot.
 */
void protocol_request_tuning();


/*
 * Clears the rx / tx activity state; call this from main after updating status LCD.
 */
void protocol_clear_comm_state();

/*
 * Gets whether a given bit is set in activity state.
 */
uint8_t protocol_comm_state(uint8_t bit);

double protocol_get_battery();

char* protocol_get_version();

void protocol_get_motors(double motors[]);

vector_t protocol_get_vector();

#endif

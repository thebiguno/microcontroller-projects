#ifndef STATUS_H
#define STATUS_H

#include "../../main.h"

#define STATUS_HEARTBEAT	1
#define STATUS_ARMED		2
#define STATUS_PROTOCOL_ERR	8

void status_init();
void status_set(uint8_t bv);
void status_clear(uint8_t bv);
void status_toggle(uint8_t bv);

#endif

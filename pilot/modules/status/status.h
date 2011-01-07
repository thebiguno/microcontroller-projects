#ifndef STATUS_H
#define STATUS_H

#include "../../main.h"

#define STATUS_HEARTBEAT	_BV(0)
#define STATUS_ARMED		_BV(1)
#define STATUS_PROTOCOL_ERR	_BV(7)

void status_init();
void status_set(uint8_t bv);
void status_clear(uint8_t bv);
void status_toggle(uint8_t bv);

#endif

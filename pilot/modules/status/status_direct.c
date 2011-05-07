#include "status.h"

static uint8_t state = 0x00;
static uint8_t error = 0x00;

void status_init() {
	// set all status pins to output
	*(&PORT_STATUS_HEARTBEAT - 0x1) |= _BV(PIN_STATUS_HEARTBEAT);
	*(&PORT_STATUS_ARMED - 0x1) |= _BV(PIN_STATUS_ARMED);
	*(&PORT_STATUS_TX - 0x1) |= _BV(PIN_STATUS_TX);
	*(&PORT_STATUS_RX - 0x1) |= _BV(PIN_STATUS_RX);
	*(&PORT_STATUS_ERR - 0x1) |= _BV(PIN_STATUS_ERR);
}

void _status_out() {
	if (error == 0) {
		PORT_STATUS_ERR &= ~_BV(PIN_STATUS_ERR);

		if (state & STATUS_HEARTBEAT) {
			PORT_STATUS_HEARTBEAT |= _BV(PIN_STATUS_HEARTBEAT);
		} else {
			PORT_STATUS_HEARTBEAT &= ~_BV(PIN_STATUS_HEARTBEAT);
		}
		if (state & STATUS_ARMED) {
			PORT_STATUS_ARMED |= _BV(PIN_STATUS_ARMED);
		} else {
			PORT_STATUS_ARMED &= ~_BV(PIN_STATUS_ARMED);
		}
		if (state & STATUS_MESSAGE_TX) {
			PORT_STATUS_TX |= _BV(PIN_STATUS_TX);
		} else {
			PORT_STATUS_TX &= ~_BV(PIN_STATUS_TX);
		}
		if (state & STATUS_MESSAGE_RX) {
			PORT_STATUS_RX |= _BV(PIN_STATUS_RX);
		} else {
			PORT_STATUS_RX &= ~_BV(PIN_STATUS_RX);
		}
	} else {
		PORT_STATUS_ERR |= _BV(PIN_STATUS_ERR);

		if (error & 0x01) {
			PORT_STATUS_HEARTBEAT |= _BV(PIN_STATUS_HEARTBEAT);
		} else {
			PORT_STATUS_HEARTBEAT &= ~_BV(PIN_STATUS_HEARTBEAT);
		}
		if (error & 0x02) {
			PORT_STATUS_ARMED |= _BV(PIN_STATUS_ARMED);
		} else {
			PORT_STATUS_ARMED &= ~_BV(PIN_STATUS_ARMED);
		}
		if (error & 0x04) {
			PORT_STATUS_TX |= _BV(PIN_STATUS_TX);
		} else {
			PORT_STATUS_TX &= ~_BV(PIN_STATUS_TX);
		}
		if (error & 0x08) {
			PORT_STATUS_RX |= _BV(PIN_STATUS_RX);
		} else {
			PORT_STATUS_RX &= ~_BV(PIN_STATUS_RX);
		}		
	}
}

void status_set(uint8_t bv) {
	state |= bv;
	_status_out();
}

void status_clear(uint8_t bv) {
	state &= ~bv;
	_status_out();
}

void status_toggle(uint8_t bv) {
	state ^= bv;
	_status_out();
}

void status_error(uint8_t condition) {
	error = condition;
	_status_out();
}
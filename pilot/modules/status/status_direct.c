#include "status.h"

uint8_t state = 0x00;

void status_init() {
	// extrapolate ddr from port
	// volatile uint8_t *_heartbeat_ddr = &PORT_STATUS_HEARTBEAT - 0x1;
	// volatile uint8_t *_armed_ddr = &PORT_STATUS_ARMED - 0x1;
	// volatile uint8_t *_tx_ddr = &PORT_STATUS_TX - 0x1;
	// volatile uint8_t *_rx_ddr = &PORT_STATUS_RX - 0x1;
	// volatile uint8_t *_err_ddr = &PORT_STATUS_ERR - 0x1;
	// 
	// // set all pins to output
	// *_heartbeat_ddr |= _BV(PIN_STATUS_HEARTBEAT);
	// *_armed_ddr |= _BV(PIN_STATUS_ARMED);
	// *_tx_ddr |= _BV(PIN_STATUS_TX);
	// *_rx_ddr |= _BV(PIN_STATUS_TX);
	// *_err_ddr |= _BV(PIN_STATUS_ERR);
	
	DDRD |= _BV(PIND2);
	DDRD |= _BV(PIND3);
	DDRD |= _BV(PIND4);
	DDRD |= _BV(PIND5);
	DDRD |= _BV(PIND6);
}

void _status_out() {
	if (state & STATUS_HEARTBEAT) {
		PORTD |= _BV(PIN_STATUS_HEARTBEAT);
	} else {
		PORTD &= ~_BV(PIN_STATUS_HEARTBEAT);
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
	if (state & STATUS_ERR) {
		PORT_STATUS_ERR |= _BV(PIN_STATUS_ERR);
	} else {
		PORT_STATUS_ERR &= ~_BV(PIN_STATUS_ERR);
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
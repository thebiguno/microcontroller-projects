#include "Status.h"

using namespace digitalcave;

Status::Status(){
	*(&LED_PORT - 1) |= RED | GREEN | BLUE;
	LED_PORT |= RED | GREEN | BLUE;	// turn off active low LED
}

void Status::poll(uint32_t time) {
	LED_PORT |= RED | GREEN | BLUE;	// turn off active low LED

	uint32_t currentPeriod = time - lastTime;
	if (currentPeriod < 250 && (status & 0x1)) {
		LED_PORT &= ~RED;
	} else if (currentPeriod < 500 && (status & 0x2)) {
		LED_PORT &= ~GREEN;
	} else if (currentPeriod < 750 && (status & 0x4)) {
		LED_PORT &= ~BLUE;
	} else if (status == 0) {
		LED_PORT &= ~(RED | GREEN | BLUE);
	}
	if (currentPeriod > 1000) {
		lastTime = time;
	}
}

void Status::batteryLow() {
	status |= 0x1;
}
void Status::batteryOK() {
	status &= ~0x1;
}
void Status::armed() {
	status |= 0x2;
}
void Status::unarmed() {
	status &= ~0x2;
}
void Status::commInterrupt() {
	status |= 0x4;
}
void Status::commOK() {
	status &= ~0x4;
}

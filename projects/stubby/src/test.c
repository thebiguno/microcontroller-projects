#include "hardware.h"
#include "hardware/pwm.h"
#include "hardware/timer0.h"

int main(void){
	//Set up the servos using the leg details + status LEDs
	volatile uint8_t *ports[1];
	uint8_t pins[1];

	ports[0] = &PORTA;
	pins[0] = PORTA6;

	pwm_init(ports, pins, 1, 20000);

	timer0_init();

	uint32_t time = 0;
	uint32_t last_time = 0;
	uint8_t phase = 0;

	while(1){
		//Tell the watchdog we are still alive
		wdt_reset();

		//Loop time
		time = timer_millis();

		if ((time - last_time) > 2000){
			pwm_set_phase(0, phase * 75, PWM_SERVO_TIMEOUT_COUNTER);
		}
	}
}

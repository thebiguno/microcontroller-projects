#include "hardware.h"
#include "hardware/pwm.h"
#include "hardware/timer0.h"
#include "Stubby.h"

int main(void){
	DDRC |= _BV(PORTC5) | _BV(PORTC6) | _BV(PORTC7);
//	DDRA |= _BV(PORTA6);
//	PORTA |= _BV(PORTA6);

	//Set up the servos using the leg details + status LEDs
	volatile uint8_t *ports[2];
	uint8_t pins[2];

	ports[0] = &PORTA;
	pins[0] = PORTA6;
	ports[1] = &PORTA;
	pins[1] = PORTA5;

	pwm_init(ports, pins, 2, 20000);

	timer0_init();

	uint32_t time = 0;
	uint32_t last_time = 0;
	uint8_t phase = 10;
	
	pwm_start();
	delay_ms(500);

	PORTC |= _BV(PORTC6);
		
	while(1){
		//Loop time
		time = timer_millis();

		if ((time - last_time) > 2000){
			PORTC ^= _BV(PORTC7);
			//PORTA ^= _BV(PORTA6);
			pwm_set_phase(0, phase++ * 75, PWM_SERVO_TIMEOUT_COUNTER);
			last_time = time;
			if (phase > 25) phase = 10;
		}
	}
}

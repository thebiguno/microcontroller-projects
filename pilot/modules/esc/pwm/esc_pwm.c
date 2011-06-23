#include "../esc.h"
#include "../../../lib/pwm/pwm.h"

// these values represent a typical ESC that runs at 50Hz, 
// turns motor off at a pulse width of 1000µs, 
// and runs at 100% duty-cycle at a pluse width of 2000µs
#define ESC_M 1000
#define ESC_B 1000
#define PULSE_SPACING 20000 // 20 ms - 50 Hz

void esc_init() {
	volatile uint8_t *ports[4];
	ports[0] = &PORT_MOTOR_A;
	ports[1] = &PORT_MOTOR_B;
	ports[2] = &PORT_MOTOR_C;
	ports[3] = &PORT_MOTOR_D;
	
	uint8_t pins[4];
	pins[0] = PIN_MOTOR_A;
	pins[1] = PIN_MOTOR_B;
	pins[2] = PIN_MOTOR_C;
	pins[3] = PIN_MOTOR_D;
	
	pwm_init(ports, pins, 4, PULSE_SPACING);
    pwm_start();
    
    double motor[4];
    motor[0] = 0;
    motor[1] = 0;
    motor[2] = 0;
    motor[3] = 0;
    esc_set(motor);
}

uint16_t _scale(double x) {
    return ESC_M * x + ESC_B;
}

void esc_set(double speed[]) {
    for (int i = 0; i < 4; i++) {
        pwm_set_phase(i, _scale(speed[i]));
    }
}

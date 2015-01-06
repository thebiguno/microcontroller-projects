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
    
	//Enable internal pullup on MISO, and set pin to input mode.  Used for calibration check / continue
	PORT_ESC_CALIBRATE_JUMPER |= _BV(PIN_ESC_CALIBRATE_JUMPER);	//pullup on
	*(&PORT_ESC_CALIBRATE_JUMPER - 0x1) &= ~_BV(PIN_ESC_CALIBRATE_JUMPER); //input mode
}

uint8_t esc_calibration_required(){
	//If MISO is low, enter ESC calibration mode
	return (! (*(&PORT_ESC_CALIBRATE_JUMPER - 0x2) & _BV(PIN_ESC_CALIBRATE_JUMPER)) );
}

/*
 *Calibrate standard PWM-driven ESCs.  The procedure for this is as follows:
 *
 * 1) Unplug all ESCs from battery, but leave the PWM signal wires connected to pilot
 * 2) Put a jumper between GND and PIN_ESC_CALIBRATE_JUMPER (on 644 this will be MISO, or PORTB6)
 * 3) Plug pilot to main battery.  The bottom 4 LEDs should be on.  The pilot is now sending PWM high (100% throttle) signals.
 * 4) Plug in the ESC which you wish to calibrate.  Depending on the ESC model, they may beep a few times to signal calibration mode.
 * 5) Following the ESC manual for exact timings, unplug the jumper.  All LEDs except for the heartbeat will be off.  Now the pilot is sending PWM low (0% throttle) messages.
 * 6) Following the ESC manual for correct sequence, you should now hear the correct confirmation beeps.  (For the Hobby King ESCs, this is 3 beeps indicating 3 cell batteries, then a long beep).
 * All done!  You can now unplug the battery, and repeat steps 1 - 6 for the other ESCs. 
 */
void esc_calibrate(){
	//Set all LEDs on
	status_set(STATUS_HEARTBEAT);
	status_set(STATUS_ARMED);
	status_set(STATUS_MESSAGE_TX);
	status_set(STATUS_MESSAGE_RX);

	double motor[4];
	
	//Turn throttle high
	motor[0] = 1.0;
	motor[1] = 1.0;
	motor[2] = 1.0;
	motor[3] = 1.0;
	esc_set(motor);
	
	//Wait for MOSI to be high again (i.e., the user pulls the jumper from MOSI to gnd)
	while (! (*(&PORT_ESC_CALIBRATE_JUMPER - 0x2) & _BV(PIN_ESC_CALIBRATE_JUMPER)) ){
		;
	}
	
	//Set status to heartbeat solid on
	status_clear(STATUS_ARMED);
	status_clear(STATUS_MESSAGE_TX);
	status_clear(STATUS_MESSAGE_RX);
	
	//Set throttle low
	motor[0] = 0.0;
	motor[1] = 0.0;
	motor[2] = 0.0;
	motor[3] = 0.0;		
	esc_set(motor);
	
	//End program
	while (1){
		;
	}
}

uint16_t _scale(double x) {
    return ESC_M * x + ESC_B;
}

void esc_set(double speed[]) {
    for (int i = 0; i < 4; i++) {
        pwm_set_phase(i, _scale(speed[i]));
    }
}

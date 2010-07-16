#ifndef PWM_H
#define PWM_H

#include <avr/io.h>
#include <avr/interrupt.h>

//The absolute maximum number of pins defined.  The actual 'used' count is passed
// into pwm_init, and there is no problem with having a larger number here than is
// actually used (other than a bit of wasted SRAM space for pre-defined arrays).
// By default we set this to 8 as that is a decently large number, but not so huge
// that tons of excess SRAM is wasted.  If you are low on memory, feel free to 
// redefine this in your own program (before including pwm.h).
#ifndef PWM_MAX_PINX
#define PWM_MAX_PINS 8
#endif

/*
 * Initializes the PWM library at a given period, using the specified ports.
 * You can have up to PWM_MAX_PINS pins defined here. 
 * 
 * To initialize this, you must pass in references to each of the PORTs, DDRs, and
 * the values of each pin.  You can do it like this:
 * 
 *    volatile uint8_t *ports[8];
 *    ports[0] = &PORTB;
 *    ports[1] = &PORTD;
 *    ....
 *    ports[5] = &PORTD;
 *
 *    volatile uint8_t *ddrs[8];
 *    ddrs[0] = &DDRB;
 *    ....
 *
 *    uint8_t pins[8];
 *    pins[0] = 1;
 *    ....
 *    pins[5] = 3;
 *
 *    pwm_init(ports, ddrs, pins, 6, 20000);
 *
 * While the code to do this is uglier than if it were to just be a function 
 * call, by allowing an arbitrary number of PWM pins, it is best to do it like this.
 */
void pwm_init(volatile uint8_t *ports[],
				volatile uint8_t *ddrs[],
				uint8_t pins[],
				uint8_t count,
				uint16_t period);
				
				
/*
 * Sets the phase of the pin at the given index to the specified value.  The index
 * matches the index used for the arrays in pwm_init().
 */
void pwm_set(uint8_t index, uint16_t phase);


#endif

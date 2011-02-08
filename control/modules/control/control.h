#ifndef CONTROL_H
#define CONTROL_H

#include <stdlib.h>
#include <avr/io.h>
#include <math.h>
#include "../../main.h"

/*
 * Control hardware must implement these functions
 */
 
/*
 * Initialize hardware (will be called prior to main loop)
 */
void control_init();

/*
 * Get the control data struct.  
 * 
 * Pitch value is in radians -- 0 is level, negative values are pitching 
 * down / forward, positive are pitching up / back.
 * 
 * Roll value in radians -- 0 is level, negative values are rolling left, 
 * positive are rolling right.
 *
 * Yaw rate in radians / second -- 0 is straight, negative values are 
 * yawing left, positive are yawing right.
 * 
 * Throttle value -- 0 is neutral / stopped, 1 is full throttle, -1 is reverse thrust full throttle.
 *
 */

/*
 * Updates the state of the controller.  
 * Must be called before the control_read methods in order to obtain fresh data.
 */
void control_update();

/*
 * Returns Throttle, Pitch, Roll, and Yaw
 */
control_t control_read_analog();
/*
 * Returns the value of all buttons.
 */
uint16_t control_read_buttons();


/*
 * Define the position of buttons in the result of the getButtons() method.
 */
#define POWER				0x0001
//#define 					0x0002
#define RESET_ATTITUDE		0x0004
#define CALIBRATE			0x0008
#define MODE_AEROBATIC 		0x0010
#define MODE_SPORT 			0x0020
#define MODE_STABLE 		0x0040
#define MODE_HOVER 			0x0080
//#define 					0x0100
//#define 					0x0200
//#define 					0x0400
//#define 					0x0800
//#define 					0x1000
//#define 					0x2000
//#define 					0x4000
//#define 					0x8000


#endif

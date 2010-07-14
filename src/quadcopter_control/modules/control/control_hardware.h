#include <stdlib.h>
#include <avr/io.h>


/*
 * Control hardware must implement these functions
 */
 
/*
 * Initialize hardware (will be called prior to main loop)
 */
void init_hardware();

/*
 * Get pitch value -- 0 is level, negative values are 
 * pitching down / forward, positive are pitching up / back
 */
uint16_t get_pitch();

/*
 * Get roll value -- 0 is level, negative values are 
 * rolling right, positive are rolling left
 */
uint16_t get_roll();


/*
 * Get yaw value -- 0 is straight, negative values are 
 * yawing right, positive are yawing left
 */
uint16_t get_yaw();


/*
 * Get throttle value -- 0 is stopped, negative values are 
 * negative thrust (motors running backwards), positive values
 * are positive thrust.
 */
uint16_t get_throttle();


/*
 * Define the position of buttons in the result of the getButtons() method.
 */
#define POWER_ON 			0x0001
//#define 					0x0002
//#define 					0x0004
//#define 					0x0008
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

/*
 * Returns the value of all buttons.
 */
uint16_t getButtons();

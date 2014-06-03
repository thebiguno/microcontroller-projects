#ifndef STUBBY_LEG_H
#define STUBBY_LEG_H

#include <avr/io.h>
#include <stdlib.h>
#include <math.h>

#include "Stubby.h"

#define DEG2RAD (180.0 / M_PI)

//The distance from 0,0 at which legs are mounted.  This assumes a radial symmetry of all legs.
#define LEG_OFFSET		45
//The length of the leg segment between coxa and femur joints (measured between center of each joint
#define COXA_LENGTH		48
//The length of the leg segment between femur and tibia joints (measured between center of each joint)
#define FEMUR_LENGTH	20
//The length of the leg segment between tibia joint and end of the foot (measured from the center of the joint)
#define TIBIA_LENGTH	57
//The height from ground to the center of the coxa joint
#define COXA_HEIGHT		68

//The lengths of the four segments in the Tibia drive system
#define TIBIA_JOINT_TO_SERVO		30.0
#define TIBIA_SERVO_HORN			6.0
#define TIBIA_CONTROL_ROD			38.0
#define TIBIA_CONTROL_ROD_TO_JOINT	14.0

//The lengths of the four segments in the Femur drive system
#define FEMUR_JOINT_TO_SERVO		27.7
#define FEMUR_SERVO_HORN			8.7
#define FEMUR_CONTROL_ROD			26.9
#define FEMUR_CONTROL_ROD_TO_JOINT	14.0

//The lengths of the four segments in the Coxa drive system
#define COXA_JOINT_TO_SERVO			23.9
#define COXA_SERVO_HORN				8.0
#define COXA_CONTROL_ROD			29.2
#define COXA_CONTROL_ROD_TO_JOINT	13.1

//We assume a neutral offset of 1500, with even amounts on either side.  We also assume that the servo has 
// a linear travel between one end and the other.
#define MIN_PHASE	700			//Clockwise from neutral
#define NEUTRAL		1500
#define MAX_PHASE	2300		//Counter clockwise from neutral

//Maximum angle of travel for the servo, in radians (between MIN_PHASE and MAX_PHASE).  Therefore, the maximum 
// travel in each direction from neutral is half of this number.
#define SERVO_TRAVEL  2.61799

namespace digitalcave {
	/*
	 * C++ implementation of Stubby the Hexapod leg.
	 */
	class Leg {
		private:
			uint8_t index;
			
			volatile uint8_t *port[JOINT_COUNT];
			uint8_t pin[JOINT_COUNT];

			int16_t x, y, z;								//Foot co-ordinates
			double mounting_angle;							//The angle at which the leg is mounted, in degrees, relative to the X axis of a standard cartesian plane.
			int8_t offset[JOINT_COUNT];						//Calibration offset in degrees
			
			/*
			 * Abstraction of matrix rotation code.  Rotates a point at c1,c2 around an origin of o1,o2 by d degrees.
			 */
			void rotate2d(int16_t *c1, int16_t *c2, int16_t o1, int16_t o2, double angle);
			
			//Set the angle for each servo.  This includes the servo abstraction code.
			void setTibiaAngle(double angle);
			void setFemurAngle(double angle);
			void setCoxaAngle(double angle);
			
			
		public:
			/*
			 * Initializes the leg, given the specified mounting angle describing it's radial position in degrees.
			 */
			Leg(uint8_t index, volatile uint8_t *tibia_port, uint8_t tibia_pin, volatile uint8_t *femur_port, uint8_t femur_pin, volatile uint8_t *coxa_port, uint8_t coxa_pin, double mounting_angle);
			
			/*
			 * Sets the foot position, in absolute x, y, z co-ordinates.  Performs the IK calculations, the absolute angle to servo angle calculations, and
			 * sets the servo position for each of the three joints.  If given an angle outside of the valid range, it sets the leg as close as possible, and
			 * updates the variables with the actual leg location.
			 */
			void setPosition(int16_t x, int16_t y, int16_t z);
			
			/*
			 * Returns the last set foot position.  If a foot was set to an out-of-bounds location, it retrieves the corrected location.
			 */
			void getPosition(int16_t *x, int16_t *y, int16_t *z);
			
			/*
			 * Gets the offset for the given joint
			 */
			int8_t getOffset(uint8_t joint);
			
			/*
			 * Sets the offset for the given joint
			 */
			void setOffset(uint8_t joint, int8_t offset);
			
			/*
			 * Gets the specified port
			 */
			volatile uint8_t* getPort(uint8_t joint);
			
			/*
			 * Gets the specified pin
			 */
			uint8_t getPin(uint8_t joint);
	} ;
}
#endif

#ifndef STUBBY_LEG_H
#define STUBBY_LEG_H

#ifndef DEBUG_SIMULATION
#include <avr/io.h>
#else
#include "../simulation/debug.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#endif
#include <math.h>

#include "Stubby.h"

//*** See doc/diagrams.pdf for figure x.y references ***//


//The distance from 0,0 at which legs are mounted.  This assumes a radial symmetry of all legs (i.e. the distance
// is the same for all the legs).  See figure 1.1, 'leg offset'
#define LEG_OFFSET						45
//The angle offset for a single leg.  All legs are defined as a multiple of this number.  For the current design
// (radial symmetry, with the middle leg sticking straight out the side) this should be 60 degrees.  See
// figure 1.1, 'mounting angle'.
#define LEG_MOUNTING_ANGLE				(60 * M_PI / 180)

//The height from ground to the center of the coxa joint.  See figure 1.2, 'coxa height'.
#define COXA_HEIGHT						50
//The length of the leg segment between coxa and femur joints (measured between center of each joint).  See
// figure 1.2, 'coxa length'
#define COXA_LENGTH						49
//The height from the center of the coxa joint to the center of the femur joint.  See figure 1.2, 'femur height'.
#define FEMUR_HEIGHT					17.825
//The length of the leg segment between femur and tibia joints (measured between center of each joint).  See
// figure 1.2, 'femur length'
#define FEMUR_LENGTH					40
//The length of the leg segment between tibia joint and end of the foot (measured from the center of the joint)
// See figure 1.2, 'tibia length'
#define TIBIA_LENGTH					57


//The lengths of the four segments in the Tibia drive system.  See figure 1.3, segments a, b, c and d.
#define TIBIA_A							30.0
#define TIBIA_B							16.3
#define TIBIA_C							54.0
#define TIBIA_D							34.0
//The difference in angle between the desired angle and the angle between segments d and a.  See
// figure 1.3, angle 'E'
#define TIBIA_E_OFFSET_ANGLE			(9.3 * M_PI / 180)
//The angle at which the servo horn extends from the servo when a neutral PWM signal is applied.  See
// figure 1.3, angle 'N'
#define TIBIA_NEUTRAL_SERVO_ANGLE		(99.7 * M_PI / 180)

//The lengths of the four segments in the Femur drive system.  See figure 1.4, segments a, b, c and d.
#define FEMUR_A							27.7
#define FEMUR_B							16.5
#define FEMUR_C							38.5
#define FEMUR_D							34.0
//The difference in angle between the desired angle and the angle between segments d and a.  See
// figure 1.4, angle 'E'
#define FEMUR_E_OFFSET_ANGLE			(31.5 * M_PI / 180)
//The angle at which the servo horn extends from the servo when a neutral PWM signal is applied.  See
// figure 1.4, angle 'N'
#define FEMUR_NEUTRAL_SERVO_ANGLE		(116.4 * M_PI / 180)

//The lengths of the four segments in the Femur drive system.  See figure 1.5, segments a, b, c and d.
#define COXA_A							23.9
#define COXA_B							8.0
#define COXA_C							28.8
#define COXA_D							13.1
//The difference in angle between the desired angle and the angle between segments d and a.  See
// figure 1.4, angle 'E'
#define COXA_E_OFFSET_ANGLE				(120 * M_PI / 180)
//The angle at which the servo horn extends from the servo when a neutral PWM signal is applied.  See
// figure 1.4, angle 'N'
#define COXA_NEUTRAL_SERVO_ANGLE		(77 * M_PI / 180)


//Servo travel information.  We assume a neutral offset of 1500, with even amounts on either side.  We also assume that the servo has 
// a linear travel between one end and the other.
#define MIN_PHASE						700			//Clockwise from neutral
#define NEUTRAL							1500
#define MAX_PHASE						2300		//Counter clockwise from neutral

//Maximum angle of travel for the servo, in radians (between MIN_PHASE and MAX_PHASE).  Therefore, the maximum 
// travel in each direction from neutral is half of this number.
#define SERVO_TRAVEL					(150 * M_PI / 180)

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

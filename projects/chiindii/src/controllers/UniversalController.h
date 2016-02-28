#ifndef UNIVERSAL_CONTROLLER_H
#define UNIVERSAL_CONTROLLER_H

#include <UniversalControllerClient.h>
#include <FramedSerialProtocol.h>
#include <PID.h>

#define MODE_FLIGHT			0
#define MODE_TUNING			1

#define AXIS_RX				0
#define AXIS_RY				1
#define AXIS_RZ				2
#define AXIS_AX				3
#define AXIS_AY				4
#define AXIS_AZ				5
#define AXIS_T				6

#define PID_P				0
#define PID_I				1
#define PID_D				2

namespace digitalcave {
	class Chiindii; // forward declaration

	class UniversalController {
		private:
			uint8_t mode;
			uint8_t axis;	//RX,RY,RZ,AX,AY,AZ,T (throttle)
			uint8_t pid;	//P,I,D for each axis
			uint8_t rawThrottle;	//Raw throttle position (1 byte)
			
			Chiindii *chiindii;
			
			PID* getPid();	//Returns the current PID object based on axis index
			void updatePidDisplay();	//Update the display to show current PID values
			
		public:
			UniversalController(Chiindii *chiindii);

			void dispatch(FramedSerialMessage* message);
	};
}
#endif


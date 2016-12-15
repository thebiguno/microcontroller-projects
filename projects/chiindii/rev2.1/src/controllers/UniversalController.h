#ifndef UNIVERSAL_CONTROLLER_H
#define UNIVERSAL_CONTROLLER_H

#include <UniversalControllerClient.h>
#include <FramedSerialProtocol.h>
#include <PID.h>


namespace digitalcave {
	class Chiindii; // forward declaration

	class UniversalController {
		private:
			enum uc_mode_t {UC_MODE_NORMAL, UC_MODE_TUNING};
			enum tuning_axis_t {AXIS_RX, AXIS_RY, AXIS_RZ, AXIS_AX, AXIS_AY, AXIS_AZ, AXIS_T};
			enum pid_variable_t {PID_P, PID_I, PID_D};

			uc_mode_t mode;
			tuning_axis_t axis;	//RX,RY,RZ,AX,AY,AZ,T (throttle)
			pid_variable_t pid;	//P,I,D for each axis
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

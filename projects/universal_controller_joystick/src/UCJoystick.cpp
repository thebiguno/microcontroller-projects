#include <SerialTeensy.h>
#include <FramedSerialProtocol.h>
#include <UniversalControllerClient.h>

/*
 * Connect pin 0 (RX) to Bluetooth TX, and pin 1 to Bluetooth RX.
 */

using namespace digitalcave;

int main(){
	SerialTeensy serial(38400, 1);
	FramedSerialProtocol protocol(128);
	FramedSerialMessage message(0, 128);

	while (1){
		if (protocol.read(&serial, &message)) {
			uint8_t cmd = message.getCommand();
			if (cmd == MESSAGE_UC_BUTTON_PUSH || cmd == MESSAGE_UC_BUTTON_RELEASE){
				Joystick.button(message.getData()[0], cmd == MESSAGE_UC_BUTTON_PUSH ? 1 : 0);
			}
			else if (cmd == MESSAGE_UC_JOYSTICK_MOVE){
				uint8_t* sticks = message.getData();
				double adjusted[4];
			
				for (uint8_t i = 0; i < 4; i++){
					if (sticks[i] < 100){
						adjusted[i] = sticks[i] / 200.0;
					}
					else if (sticks[i] > 155){
						adjusted[i] = 0.5 + (sticks[i] - 155.0) / 200.0;
					}
					else {
						adjusted[i] = 0;
					}
				}

				Joystick.X(adjusted[2] * 1023);
				Joystick.Y(adjusted[3] * 1023);
				Joystick.Z(adjusted[0] * 1023);
			}
			else if (cmd == MESSAGE_UC_THROTTLE_MOVE){
				double throttle = message.getData()[0] / 255.0;
				Joystick.sliderLeft(throttle * 1023);
			}
		}
	}
}

#include <avr/io.h>

#include "main.h"

int main (void){
    uint8_t mode = MODE_STABLE;
    uint8_t flags = 0;
    uint16_t debounce = 0;
    
	//Init communication hardware
	comm_init();

	//Init control interface
	control_init();
	
	//Main program loop
	while (1){
		//Read controls
        control_t control = control_read_analog();
        uint16_t buttons = control_read_buttons();
        
        flags = 0x00;
        if (((debounce & POWER_ON) == 0) && buttons & POWER_ON) {
            debounce |= POWER_ON;
            flags &= POWER_ON;
        } else if (!(buttons & POWER_ON)) {
            debounce &= ~POWER_ON;
        }
        
        if (buttons & MODE_AEROBATIC) {
            mode = MODE_AEROBATIC;
        }
        
        if (buttons & MODE_STABLE) {
            mode = MODE_STABLE;
        }
        
        if (buttons & MODE_HOVER) {
            mode = MODE_HOVER;
        }
        
        if (buttons & MODE_STABLE) {
            mode = MODE_STABLE;
        }
        
        // TODO implement calibrate and reset flags
        
        // TODO should there be limits on throttle???
        if ((mode & MODE_AEROBATIC) == MODE_AEROBATIC) {
            // aerobatic / 3d mode (no limits on pitch and roll)
        } else if ((mode & MODE_STABLE) == MODE_SPORT) {
            // sport mode (roll limited to 30 deg, pitch limited to 45 deg)
            control.roll *= 0.3333;
            control.roll *= 0.5;
        } else if ((mode & MODE_HOVER) == MODE_HOVER) {
            // TODO this mode is pretty useless
            // hover mode (roll, pitch, yaw = 0)
            control.pitch = 0;
            control.roll = 0;
        } else {
            // stable mode (pitch and roll limited to 5 deg -- 0.0872664626 radians)
            control.pitch *= 0.0555;
            control.roll *= 0.0555;
        }
        
        //Send control data
        protocol_send_control(control);
    }
}


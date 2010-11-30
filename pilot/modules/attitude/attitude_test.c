#include <stdio.h>

#include "attitude.h"
#include "../../main.h"
#include "../../modules/accel/accel.h"
#include "../../modules/gyro/gyro.h"
#include "../../../lib/serial/serial.h"

int main(){
    char temp[35];
    
    serial_init(9600, 8, 0, 1);
    
    gyro_init();
    gyro_calibrate();
    
    accel_init();
    accel_calibrate();
    
    //Main program loop
    while (1) {
        vector_t g = gyro_get();
        vector_t a = accel_get();
        vector_t pv = attitude(g, a);
        
        sprintf(temp, " x=%+d,  y=%+d,  z=%+d\n\r", (int) (pv.x * 57.2957795), (int) (pv.y * 57.2957795), (int) (pv.z * 57.2957795));
        serial_write_s(temp);
    }
}


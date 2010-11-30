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
        
        sprintf(temp, "gx=%+1.2f, gy=%+1.2f, gz=%+1.2f\n\r", g.x, g.y, g.z);
        serial_write_s(temp);
        sprintf(temp, "ax=%+1.2f, ay=%+1.2f, az=%+1.2f\n\r", a.x, a.y, a.z);
        serial_write_s(temp);
        sprintf(temp, " x=%+1.2f,  y=%+1.2f,  z=%+1.2f\n\r", pv.x, pv.y, pv.z);
        serial_write_s(temp);
    }
}


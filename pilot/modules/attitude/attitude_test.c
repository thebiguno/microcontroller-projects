#include <stdio.h>

#include "attitude.h"
#include "../../main.h"
#include "../../modules/accel/accel.h"
#include "../../modules/gyro/gyro.h"
#include "../../../lib/serial/serial.h"

int main(){
    char temp[32];
    
    //Main program loop
    while (1) {
        vector_t g = gyro_get();
        vector_t a = accel_get();
        vector_t pv = attitude(g, a);
        
        sprintf(temp, "x=%+1.2f, y=%+1.2f, z=%+1.2f\n\r", pv.x, pv.y, pv.z);
        serial_write_s(temp);
    }
}


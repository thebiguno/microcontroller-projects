#include <util/delay.h>
extern "C" {
  #include "serial.h"
}


/*
 * Universal programmable / learning remote.
 */
 
void  setup(){
  serial_init(9600, 8, 0, 1);
}

void loop(){
  serial_write_s("abcdefghijklmnopqrstuvwxyz\n");
  _delay_ms(1000);
}

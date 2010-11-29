#ifndef ESC_H
#define ESC_H

#include "../../main.h"

void esc_init();

/*
 * send the motor speed percentages (0 .. 1) to the speed controllers
 */
void esc_set(double speed[]);

#endif

#ifndef USB_H
#define USB_H

#include "lib/usb/rawhid.h"

#include "State.h"

#define MESSAGE_SEND_ACTUAL				0
#define MESSAGE_SEND_RAW				1
#define MESSAGE_REQUEST_SETPOINT		2
#define MESSAGE_SEND_SETPOINT			3

void usb_send_actual_values();

void usb_check_for_updates();

#endif
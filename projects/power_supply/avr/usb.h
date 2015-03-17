#ifndef USB_H
#define USB_H

#include "lib/usb/rawhid.h"

#include "State.h"

//All messages start with the message type (as defined below).  The remainder of the message
// is the message body, specific to the type of message.  In the documentation below,
// we use the form "[foo:4][bar:1]" to indicate that the message body consists of 
// a value 'foo' using 4 bytes, then a value 'bar' using 1 byte.

//Computer -> PSU: Request number of channels: []
//PSU -> Computer: Send number of channels: [channel:1]
#define MESSAGE_CHANNELS				0

//Computer -> PSU: Request actual measured values for given channel: [channel:1]
//PSU -> Computer: Send actual measured values: [channel:1][voltage:2][current:2]
#define MESSAGE_ACTUAL					1

//Computer -> PSU: Request actual raw values for given channel: [channel:1]
//PSU -> Computer: Send actual raw values: [channel:1][voltage:2][current:2]
#define MESSAGE_ACTUAL_RAW				2

//Computer -> PSU: Request setpoints for given channel: [channel:1]
//PSU -> Computer: Send setpoints for given channel: [channel:1][voltage:2][current:2]
#define MESSAGE_SETPOINT				3

//Computer -> PSU: Request raw setpoints for given channel: [channel:1]
//PSU -> Computer: Send raw setpoints for given channel: [channel:1][voltage:2][current:2]
#define MESSAGE_SETPOINT_RAW			4

//Computer -> PSU: Change setpoints for given channel: [channel:1][voltage:2][current:2]
//PSU -> Computer: N/A
#define MESSAGE_CHANGE_SETPOINT			5

//Computer -> PSU: Change raw setpoints for given channel: [channel:1][voltage:2][current:2]
//PSU -> Computer: N/A
#define MESSAGE_CHANGE_SETPOINT_RAW		6

void usb_dispatch();

#endif
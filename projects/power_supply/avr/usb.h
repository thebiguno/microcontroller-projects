#ifndef USB_H
#define USB_H

#include "lib/bootloader/bootloader.h"
#include "lib/usb/rawhid.h"

#include "Channel.h"
#include "Display.h"
#include "State.h"

//All messages start with the message type (as defined below).  The remainder of the message
// is the message body, specific to the type of message.  In the documentation below,
// we use the form "[foo:4][bar:1]" to indicate that the message body consists of 
// a value 'foo' using 4 bytes, then a value 'bar' using 1 byte.

//Computer -> PSU: Request number of channels: []
//PSU -> Computer: Send number of channels and bitmask for negative channels: [channel:1][negatives:1]
#define MESSAGE_CHANNELS				1

//Computer -> PSU: Request actual measured values for given channel: [channel:1]
//PSU -> Computer: Send actual measured values: [channel:1][voltage:2][current:2]
#define MESSAGE_ACTUAL					2

//Computer -> PSU: Request actual raw values for given channel: [channel:1]
//PSU -> Computer: Send actual raw values: [channel:1][voltage:2][current:2]
#define MESSAGE_ACTUAL_RAW				3

//Computer -> PSU: Request setpoints for given channel: [channel:1]
//PSU -> Computer: Send setpoints for given channel: [channel:1][voltage:2][current:2]
#define MESSAGE_SETPOINT				4

//Computer -> PSU: Request raw setpoints for given channel: [channel:1]
//PSU -> Computer: Send raw setpoints for given channel: [channel:1][voltage:2][current:2]
#define MESSAGE_SETPOINT_RAW			5

//Computer -> PSU: Change setpoints for given channel: [channel:1][voltage:2][current:2]
//PSU -> Computer: Confirm raw setpoints: [channel:1][voltage:2][current:2]
#define MESSAGE_CHANGE_SETPOINT			6

//Computer -> PSU: Change raw setpoints for given channel: [channel:1][voltage:2][current:2]
//PSU -> Computer: Confirm raw setpoints: [channel:1][voltage:2][current:2]
#define MESSAGE_CHANGE_SETPOINT_RAW		7

//Computer -> PSU: Get calibration for given channel / target: [channel:1][target:1]
//PSU -> Computer: Confirm raw setpoints: [channel:1][target:1][calibration_value:4]
//Target is one of the calibration targets defined below
#define MESSAGE_GET_CALIBRATION			8

//Computer -> PSU: Change calibration for given channel / target: [channel:1][target:1][calibration_value:4]
//PSU -> Computer: Confirm raw setpoints: [channel:1][target:1][calibration_value:4]
//Target is one of the calibration targets defined below
#define MESSAGE_SET_CALIBRATION			9

//Computer -> PSU: Set DAC addresses: [old_dac_number:1][new_dac_number:1]
//PSU -> Computer: Confirm completion: [new_dac_number:1]
//Before running this command, you need to plug the DAC that you wish to program into 
// PORTB0 (encoder channel 1A).  When this command is received, the AVR will attempt to program
// the DAC that is connected to PORTB0 with the specified DAC number.  Returns a message
// with the requested DAC number if successful, or with dac_number 0xFF if an error occurred.
#define MESSAGE_CONFIGURE_DAC_ADDRESS	10

//Computer -> PSU: Set specified DAC to maximum value as AREF: [dac_number:1][dac_channel:1]
//PSU -> Computer: Confirm completion: [dac_number:1]
//This sets the specified DAC number / channel to the max value, to be used as AREF.
// Manually do this once, on a DAC channel that is not in use.
#define MESSAGE_CONFIGURE_AREF			11

//Computer -> PSU: Jump to bootloader in preparation for uploading new firmware: []
// PSU -> Computer: N/A
// Once this is completed, dfu command line tool can be used to flash new program.
// After programming, reset is done via a hard power cycle.
#define MESSAGE_BOOTLOADER_JUMP			12

#define TARGET_VOLTAGE_ACTUAL_SLOPE			0
#define TARGET_VOLTAGE_ACTUAL_OFFSET		1
#define TARGET_VOLTAGE_SETPOINT_SLOPE		2
#define TARGET_VOLTAGE_SETPOINT_OFFSET		3
#define TARGET_CURRENT_ACTUAL_SLOPE			4
#define TARGET_CURRENT_ACTUAL_OFFSET		5
#define TARGET_CURRENT_SETPOINT_SLOPE		6
#define TARGET_CURRENT_SETPOINT_OFFSET		7

void usb_dispatch();

#endif
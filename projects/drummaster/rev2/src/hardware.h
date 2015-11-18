#ifndef HARDWARE_H
#define HARDWARE_H

//The base potentiometer address.  Add bits 0..2 for A0-A2 pins.
#define POT_ADDRESS						0x28

#define MOSI							7
#define MISO							12
#define SCK								14
#define CS_FLASH						15
#define CS_SD							10

//Pin assignments
#define MUX0							0
#define MUX1							1
#define MUX2							2
#define MUX3							3
#define DRAIN_EN						4
#define ADC_EN							5
#define ADC_INPUT						A14

//The MUXs we use are active low for enable pin... 
#define MUX_ENABLE						0
#define MUX_DISABLE						1

//The number of ADC channels on the system.  For the DrumMaster Rev 2.0 PCB, this is 12.
#define CHANNEL_COUNT					12
//The number of pads in the system.  This will probably be CHANNEL_COUNT - 1, since the hihat takes two channels
#define PAD_COUNT						(CHANNEL_COUNT - 1)

//EEPROM starting addresses
//We need to persist 12x2 bytes for the pots; this is from address 0x00 to 0x18
#define EEPROM_POTENTIOMETER			0x00
//Main volume is stored as a single byte
#define EEPROM_MAIN_VOLUME				0x1A
//Per-pad volumes are stored as 11x1 bytes, from address 0x20 to 0x2B
#define EEPROM_CHANNEL_VOLUME			0x20
//Store the currently loaded kit name.  We use this to refer back to the SD card so that we can
// write things like channel volume to disk as well as EEPROM.  This is 32 bytes from address 0x30 to 0x50
#define EEPROM_KIT_NAME					0x30

#endif

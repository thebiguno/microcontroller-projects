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
//ADC input
#define ADC_INPUT						A14
//Data lines shared for MUX and display
#define MUX0							0
#define MUX1							1
#define MUX2							2
#define MUX3							3
//MUX enable
#define DRAIN_EN						4
#define ADC_EN							5
//Display
#define PIN_RS							16
#define PIN_E							17
//Encoder
#define ENC_PUSH						8
#define ENC_A							21
#define ENC_B							20

//The MUXs we use are active low for enable pin... 
#define MUX_ENABLE						0
#define MUX_DISABLE						1

//Display dimensions
#define DISPLAY_COLS					20
#define DISPLAY_ROWS					4

//The number of blocks to allocate to audio memory
#define AUDIO_MEMORY					32

//The total number of samples allowed in the system.  Currently 10 or possibly 11 seems to be right; more than this 
// can cause skips when playing back.  If this is set to more than 14, you need to change how the
// mixer is set up, since currently we are using a 16 Sample mixer with two Samples reserved for 
// passing through i2s audio from an external source.
#define SAMPLE_COUNT					11

//The number of ADC channels on the system.  For the DrumMaster Rev 2.0 PCB, this is 12.
#define CHANNEL_COUNT					12
//The number of pads in the system.  This will probably be CHANNEL_COUNT - 1, since the hihat takes two channels
#define PAD_COUNT						(CHANNEL_COUNT - 1)

//EEPROM starting addresses
//We need to persist 12x2 bytes for the pots; this is from address 0x00 to 0x18
#define EEPROM_POTENTIOMETER			0x00
//Main volume is stored as a single byte
#define EEPROM_MAIN_VOLUME				0x1A
//Line in volume is stored as a single byte
#define EEPROM_LINE_IN_VOLUME			0x1B
//Store the currently loaded kit index
#define EEPROM_KIT_INDEX				0x1C

//Per-pad volumes are stored as 11x1 bytes, starting at address 0x20 to 0x2B.  They are indexed by kit number,
// with kit 0 starting at 0x20, kit 1 starting at 0x20 + PAD_COUNT, and in general starting at 
// EEPROM_PAD_VOLUME + (PAD_COUNT * kit_index).
#define EEPROM_PAD_VOLUME				0x20

#endif

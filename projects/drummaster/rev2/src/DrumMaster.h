#ifndef DRUMMASTER_H
#define DRUMMASTER_H

#include <SD.h>
#include <SerialFlash.h>
#include <SPI.h>

#include "pad/Pad.h"
#include "MenuState.h"

#include "menu/MainMenu.h"
#include "menu/MainVolume.h"
#include "menu/LoadSamples.h"
#include "menu/CalibratePads.h"
#include "menu/CalibratePad.h"

#define PAD_COUNT					11

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

#endif
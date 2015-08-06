#include "DrumMaster.h"

using namespace digitalcave;

//Pin assignments
#define MUX0						0
#define MUX1						1
#define MUX2						2
#define MUX3						3
#define DRAIN_EN					4
#define ADC_EN						5
#define ADC_INPUT					A14

//Constants
#define MIN_VALUE					10

Samples samples;
AudioControlSGTL5000 control;

MainMenu* mainMenu;
MainVolume* mainVolume;
LoadSamples* loadSamples;

MenuState* menuState;


ADC adc;


uint8_t readDrum(uint8_t channel){
	if (channel >= 8) return 0;		//TODO Remove on real board; breadboard MUX only have 8 inputs
	
	//Disable both MUXs
	digitalWriteFast(ADC_EN, 0);
	digitalWriteFast(DRAIN_EN, 0);
	
	delayMicroseconds(1);
	
	//Set channel
	digitalWriteFast(MUX0, channel & 0x01);
	digitalWriteFast(MUX1, channel & 0x02);
	digitalWriteFast(MUX2, channel & 0x04);
	digitalWriteFast(MUX3, channel & 0x08);

	delayMicroseconds(1);

	//Enable ADC MUX, read value, and disable MUX again
	digitalWriteFast(ADC_EN, 1);
	delayMicroseconds(10);
	uint8_t result = adc.analogRead(ADC_INPUT);
	digitalWriteFast(ADC_EN, 0);
	
	if (result > MIN_VALUE){
		//Reset the peak value
		digitalWriteFast(DRAIN_EN, 1);
		delay(1);
	}
	
	return result;
}

int main(){
	//Turn on the audio chip
	control.enable();
	control.volume(0.5);
	
	//Enable pins
	pinMode(MUX0, OUTPUT);
	pinMode(MUX1, OUTPUT);
	pinMode(MUX2, OUTPUT);
	pinMode(MUX3, OUTPUT);
	pinMode(DRAIN_EN, OUTPUT);
	pinMode(ADC_EN, OUTPUT);
	
	//Encoder pushbutton
	pinMode(ENC_PUSH, INPUT_PULLUP);
	
	//Set up ADC
	adc.setResolution(8);
	
	//Audio shield SD / flash setup
	SPI.setMOSI(MOSI);
	SPI.setMISO(MISO);
	SPI.setSCK(SCK);
	SerialFlash.begin(CS_FLASH);
	SD.begin(CS_SD);
	
	mainMenu = new MainMenu();
	mainVolume = new MainVolume();
	loadSamples = new LoadSamples();

	menuState = new MenuState(mainMenu);
	
	//Turn on the audio chip
	control.enable();
	//control.volume(state.get_volume());
	control.volume(0.5);
	
	uint8_t channel = 0;
	while (1){
		menuState->poll();
		
		uint8_t value = readDrum(channel);
		if (value > MIN_VALUE){
			samples.play(channel, value);
		}
		channel = (channel + 1) & 0x0F;
	}
}



#include <Audio.h>
#include <Bounce.h>
#include <SD.h>
#include <SPI.h>

AudioPlaySdRaw samples[16];
AudioMixer4 mixers[4];
AudioMixer4 masterMixer;
AudioOutputI2S           output;
AudioConnection          patchCord0(samples[0], 0, mixers[0], 0);
AudioConnection          patchCord1(samples[1], 0, mixers[0], 1);
AudioConnection          patchCord2(samples[2], 0, mixers[0], 2);
AudioConnection          patchCord3(samples[3], 0, mixers[0], 3);
AudioConnection          patchCord4(samples[4], 0, mixers[1], 0);
AudioConnection          patchCord5(samples[5], 0, mixers[1], 1);
AudioConnection          patchCord6(samples[6], 0, mixers[1], 2);
AudioConnection          patchCord7(samples[7], 0, mixers[1], 3);
AudioConnection          patchCord8(samples[8], 0, mixers[2], 0);
AudioConnection          patchCord9(samples[9], 0, mixers[2], 1);
AudioConnection          patchCord10(samples[10], 0, mixers[2], 2);
AudioConnection          patchCord11(samples[11], 0, mixers[2], 3);
AudioConnection          patchCord12(samples[12], 0, mixers[3], 0);
AudioConnection          patchCord13(samples[13], 0, mixers[3], 1);
AudioConnection          patchCord14(samples[14], 0, mixers[3], 2);
AudioConnection          patchCord15(samples[15], 0, mixers[3], 3);
AudioConnection          patchCord16(mixers[0], 0, masterMixer, 0);
AudioConnection          patchCord17(mixers[1], 0, masterMixer, 1);
AudioConnection          patchCord18(mixers[2], 0, masterMixer, 2);
AudioConnection          patchCord19(mixers[3], 0, masterMixer, 3);
AudioConnection          patchCord20(masterMixer, 0, output, 0);
AudioConnection          patchCord21(masterMixer, 0, output, 1);
AudioControlSGTL5000     sgtl5000;

//Index into samples array
uint8_t sampleCounter = 0;

Bounce button0 = Bounce(15, 25);

int main(){
	delay(1000);
	Serial.println("Starting...");
	
	//Audio shield SPI setup
	SPI.setMOSI(7);
	SPI.setMISO(12);
	SPI.setSCK(14);
	
	AudioMemory(16);
	
	sgtl5000.enable();
	sgtl5000.volume(0.5);
	
	pinMode(15, INPUT_PULLUP);

	if (!(SD.begin(10))) {
		// stop here, but print a message repetitively
		while (1) {
			Serial.println("Unable to access the SD card");
			delay(500);
		}
	}
	
	while (1){
		button0.update();
		
		if (button0.fallingEdge()) {
			Serial.print("Button push sample ");
			Serial.println(sampleCounter);
			samples[sampleCounter].stop();
			samples[sampleCounter].play("00.RAW");
			sampleCounter = (sampleCounter + 1) & 0x03;
			
			Serial.print("CPU: ");
			Serial.print(AudioProcessorUsage());
			Serial.print(",");
			Serial.print(AudioProcessorUsageMax());
			Serial.print("    ");
			Serial.print("Memory: ");
			Serial.print(AudioMemoryUsage());
			Serial.print(",");
			Serial.print(AudioMemoryUsageMax());
			Serial.println();
		}
		
		delay(5);
	}
}



#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=96,423
AudioMixer4              mixer1;         //xy=249,440
AudioEffectGranular      granular3;      //xy=576,674
AudioEffectGranular      granular2;      //xy=577,453
AudioEffectGranular      granular1;      //xy=604,363
AudioEffectReverb        reverb1;        //xy=748,361
AudioEffectReverb        reverb2;        //xy=749,428
AudioEffectDelay         delay1;         //xy=749,514
AudioAmplifier           amp1;           //xy=901,367
AudioAmplifier           amp3;           //xy=911,668
AudioMixer4              mixer3;         //xy=916,464
AudioAmplifier           amp2;           //xy=1063,457
AudioAnalyzePeak         peak1;          //xy=1071,693
AudioMixer4              mixer2;         //xy=1271,459
AudioFilterBiquad        biquad1;        //xy=1441,498
AudioOutputI2S           i2s2;           //xy=1608,456
AudioConnection          patchCord1(i2s1, 0, mixer1, 0);
AudioConnection          patchCord2(i2s1, 1, mixer1, 1);
AudioConnection          patchCord3(mixer1, granular1);
AudioConnection          patchCord4(mixer1, granular2);
AudioConnection          patchCord5(mixer1, granular3);
AudioConnection          patchCord6(granular3, amp3);
AudioConnection          patchCord7(granular2, reverb2);
AudioConnection          patchCord8(granular2, delay1);
AudioConnection          patchCord9(granular1, reverb1);
AudioConnection          patchCord10(reverb1, amp1);
AudioConnection          patchCord11(reverb2, 0, mixer3, 0);
AudioConnection          patchCord12(delay1, 0, mixer3, 1);
AudioConnection          patchCord13(delay1, 1, mixer3, 2);
AudioConnection          patchCord14(delay1, 2, mixer3, 3);
AudioConnection          patchCord15(amp1, 0, mixer2, 0);
AudioConnection          patchCord16(amp3, 0, mixer2, 2);
AudioConnection          patchCord17(amp3, peak1);
AudioConnection          patchCord18(mixer3, amp2);
AudioConnection          patchCord19(amp2, 0, mixer2, 1);
AudioConnection          patchCord20(mixer2, biquad1);
AudioConnection          patchCord21(mixer2, 0, i2s2, 0);
AudioConnection          patchCord22(mixer2, 0, i2s2, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=313,584
// GUItool: end automatically generated code












//#define DEBUG

#include <Servo.h> 
 
Servo mouthServo;


#define GRANULAR_BUFFER_LENGTH 25600  // enough for 290 ms at 44.1 kHz
int16_t granularMemory1[GRANULAR_BUFFER_LENGTH];
int16_t granularMemory2[GRANULAR_BUFFER_LENGTH];
int16_t granularMemory3[GRANULAR_BUFFER_LENGTH];

// Number of samples in each delay line
#define CHORUS_BUFFER_LENGTH (128 * AUDIO_BLOCK_SAMPLES)
// Allocate the delay lines for left and right channels
short chorusMemory1[CHORUS_BUFFER_LENGTH];


void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  #endif

  AudioMemory(1000);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.0);
  sgtl5000_1.muteHeadphone();
  sgtl5000_1.muteLineout();
  sgtl5000_1.dacVolumeRamp();
  sgtl5000_1.inputSelect(AUDIO_INPUT_MIC);
  sgtl5000_1.adcHighPassFilterEnable();
  sgtl5000_1.audioPreProcessorEnable();
  sgtl5000_1.audioPostProcessorEnable();
  //sgtl5000_1.enhanceBassEnable();
  //sgtl5000_1.eqBands(0.5, 0.0, -0.5, -1, -1);
  sgtl5000_1.autoVolumeControl(2, 0, 1, -18, 40, 100);
  sgtl5000_1.autoVolumeEnable();

  mixer1.gain(0, 1.5);
  mixer1.gain(1, 1.5);

  granular1.begin(granularMemory1, GRANULAR_BUFFER_LENGTH);
  granular1.setSpeed(0.3);
  granular1.beginPitchShift(5);

  granular2.begin(granularMemory2, GRANULAR_BUFFER_LENGTH);
  granular2.setSpeed(1.4);
  granular2.beginPitchShift(5);

  granular3.begin(granularMemory3, GRANULAR_BUFFER_LENGTH);
  granular3.setSpeed(0.9);
  granular3.beginPitchShift(5);

  reverb1.reverbTime(5);
  reverb2.reverbTime(2);

  delay1.delay(0, 200);
  delay1.delay(1, 400);
  delay1.delay(2, 600);

  mixer3.gain(1, 0.5);
  mixer3.gain(2, 0.3);
  mixer3.gain(3, 0.1);

  amp1.gain(1.2);
  amp2.gain(0.7);
  amp3.gain(1.7);

  //Finally, after all the setup, we slowly enable the volume to prevent loud startup noises
  sgtl5000_1.unmuteHeadphone();

  mouthServo.attach(22); //Servo uses pin 22
  mouthServo.write(20);

  for (int i = 0; i < 16; i++){
    sgtl5000_1.volume(i * 0.05);  //Max volume of 0.75
    sgtl5000_1.micGain(i);  //Max mic gain of 15
    delay(100);
  }

  delay(100);
  peak1.read(); //Eliminate any startup noises from moving the mouth
  delay(100);
}

void loop() {
  if (peak1.available()){
    double val = peak1.read();

    //Use a logarithmic function to make the mouth open faster than it would when linear.
    double scaledMouthPosition = 1.25 * log(10 * (val + 0.2)) - 1;
    if (scaledMouthPosition < 0.2){
      scaledMouthPosition = 0;
    }
    else if (scaledMouthPosition > 1){
      scaledMouthPosition = 1;
    }
    #ifdef DEBUG
    if (scaledMouthPosition > 0.1){
      Serial.println(scaledMouthPosition);
    }
    #endif
    mouthServo.write((scaledMouthPosition * 80) + 20);
  }

  delay(100);
}

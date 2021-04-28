#include <Encoder.h>
#include <Bounce.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2sIn;          //xy=309,242
AudioMixer4              mixer1;         //xy=468,255
AudioRecordQueue         queueRecord;    //xy=607,255
AudioSynthSimpleDrum     metronome;      //xy=800,368
AudioPlayQueue           queuePlay;      //xy=830,259
AudioMixer4              mixer2;         //xy=982,388
AudioOutputI2S           i2sOut;         //xy=1164,268
AudioConnection          patchCord1(i2sIn, 0, mixer1, 0);
AudioConnection          patchCord2(i2sIn, 1, mixer1, 1);
AudioConnection          patchCord3(mixer1, queueRecord);
AudioConnection          patchCord4(metronome, 0, mixer2, 1);
AudioConnection          patchCord5(queuePlay, 0, i2sOut, 0);
AudioConnection          patchCord6(queuePlay, 0, mixer2, 0);
AudioConnection          patchCord7(mixer2, 0, i2sOut, 1);
AudioControlSGTL5000     control;        //xy=359,372
// GUItool: end automatically generated code


//Looper state
Encoder beatsPerMinuteKnob(34, 33);
const uint8_t timeSignatureButtonPin = 35;
Bounce timeSignatureButton = Bounce(timeSignatureButtonPin, 5); //5ms debounce time

const uint8_t looperButtonPin = 41;
Bounce looperButton = Bounce(looperButtonPin, 5); //5ms debounce time
uint8_t looperState = 0;    //0 = playback / stop, 1 = recording

//Looper buffer and index;
uint32_t looperBufferIndex = 0;
uint32_t looperBufferLength = 0;
EXTMEM uint8_t looperBuffer[6400000];

//Metronome state
uint8_t timeSignatureCount = 4;   //The top of the time signature
int16_t beatsPerMinute = 120;           //Tempo
elapsedMillis timeSinceLastBeat = 0;    //Used to time between each beat
uint8_t beatCounter = 0;    //Used to keep state of full notes vs. fractional notes.


void setup() {
  AudioMemory(128);
  
  control.enable();
  control.inputSelect(AUDIO_INPUT_LINEIN);
  control.volume(0.6);
  
  metronome.length(50);
  metronome.pitchMod(0.8);

  pinMode(timeSignatureButtonPin, INPUT_PULLUP);
  pinMode(looperButtonPin, INPUT_PULLUP);

  beatsPerMinuteKnob.write(beatsPerMinute);

  Serial.begin(115200);
}

void loop() { 
  if (beatsPerMinute != beatsPerMinuteKnob.read()){
    Serial.print(beatsPerMinute);
    Serial.println(" BPM");
  }
  beatsPerMinute = beatsPerMinuteKnob.read();
  if (beatsPerMinute < 60){
    beatsPerMinute = 60;
    beatsPerMinuteKnob.write(beatsPerMinute);
  }
  else if (beatsPerMinute > 360){
    beatsPerMinute = 360;
    beatsPerMinuteKnob.write(beatsPerMinute);
  }

  if (timeSignatureButton.update()){
    if (timeSignatureButton.fallingEdge()){
      timeSignatureCount++;
      timeSignatureCount = timeSignatureCount % 9;  //Keep time signature between 0 and 8 inclusive
    }
    Serial.print(timeSignatureCount);
    Serial.println(" Beats");
  }
  
  if (timeSignatureCount > 0){  //Set time signature to 0 to disable metronome
    uint16_t beatPeriod = (1 / (beatsPerMinute / 60.0) * 1000);
    if (timeSinceLastBeat >= beatPeriod){
      timeSinceLastBeat -= beatPeriod;
      metronome.frequency(beatCounter == 0 ? 400 : 200);
      metronome.noteOn();
      beatCounter = (beatCounter + 1) % timeSignatureCount;
      if (looperState){
        Serial.print("Recording ");
        Serial.print(looperBufferIndex);
        Serial.println(" bytes");
      }
    }
  }

  if (looperButton.update()){
    if (looperButton.fallingEdge()){
      if (looperState){
        Serial.print("Playing back ");
        Serial.print(looperBufferIndex);
        Serial.println(" bytes");
        queueRecord.end();
        looperState = 0;
        looperBufferLength = looperBufferIndex;
        looperBufferIndex = 0;
      }
      else {
        Serial.println("Recording");
        queueRecord.begin();
        looperBufferIndex = 0;
        looperBufferLength = 0;
        looperState = 1;
      }
    }
  }

  if (looperState){
    //Recording
    while(queueRecord.available()){
      memcpy(looperBuffer + looperBufferIndex, queueRecord.readBuffer(), 256);
      looperBufferIndex += 256;
      queueRecord.freeBuffer();
    }
  }
  else {
    //Playing back
    if (looperBufferLength > 0 && queuePlay.available()){
      for (uint8_t i = 0; i < 8 && queuePlay.available(); i++){
        memcpy(queuePlay.getBuffer(), looperBuffer + looperBufferIndex, 256);
        queuePlay.playBuffer();
        looperBufferIndex += 256;
        if (looperBufferIndex >= looperBufferLength){
          looperBufferIndex = 0;
          Serial.println("Playing back buffer"); 
        }
      }
    }
  }
}

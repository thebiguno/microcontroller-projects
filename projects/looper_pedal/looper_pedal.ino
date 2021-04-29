#include <Encoder.h>
#include <Bounce.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioPlayQueue           queuePlay;      //xy=100,111
AudioInputI2S            i2sIn;          //xy=100,277
AudioAmplifier           looperVolume;           //xy=278,182
AudioMixer4              mixer1;         //xy=485,281
AudioSynthSimpleDrum     metronome;      //xy=635,211
AudioRecordQueue         queueRecord;    //xy=699,345
AudioMixer4              mixer2;         //xy=859,256
AudioOutputI2S           i2sOut;         //xy=1060,183
AudioConnection          patchCord1(queuePlay, looperVolume);
AudioConnection          patchCord2(i2sIn, 0, mixer1, 1);
AudioConnection          patchCord3(i2sIn, 1, mixer1, 2);
AudioConnection          patchCord4(looperVolume, 0, i2sOut, 0);
AudioConnection          patchCord5(looperVolume, 0, mixer1, 0);
AudioConnection          patchCord6(mixer1, queueRecord);
AudioConnection          patchCord7(mixer1, 0, mixer2, 1);
AudioConnection          patchCord8(metronome, 0, mixer2, 0);
AudioConnection          patchCord9(mixer2, 0, i2sOut, 1);
AudioControlSGTL5000     control;        //xy=344,466
// GUItool: end automatically generated code





//Looper state
Encoder beatsPerMinuteKnob(34, 33);
const uint8_t timeSignatureButtonPin = 35;
Bounce timeSignatureButton = Bounce(timeSignatureButtonPin, 5); //5ms debounce time

const uint8_t looperButtonPin = 41;
Bounce looperButton = Bounce(looperButtonPin, 5); //5ms debounce time
enum record_state {
  play,       //Playing back the buffer repeatedly
  record,     //Actively recording
  start_record,       //Ready to start recording as soon as the next metronome click happens.  Only applicable when timeSignatureCount != 0.
  stop_record         //Ready to stop recording as soon as the next metronome click happens.  Only applicable when timeSignatureCount != 0.
};
record_state state = play;

//Recording / playback buffers
extern "C" uint8_t external_psram_size;
//Buffer size in bytes
uint32_t bufferSize = external_psram_size * 1048576 / 2;
//uint32_t bufferSize = 1024 * 512;
struct Buffer {
  uint32_t index = 0;
  uint32_t length = 0;
  uint8_t *buffer;
} buffer;
struct Buffer recordBuffer;
struct Buffer playBuffer;

//Metronome state
uint8_t timeSignatureCount = 4;   //The top of the time signature
int16_t beatsPerMinute = 120;           //Tempo
elapsedMillis timeSinceLastBeat = 0;    //Used to time between each beat


void setup() {
  //This is evil.  We are essentially using unallocated memory.  We need to fix this.
  recordBuffer.buffer = (uint8_t *)(0x70000000);
  playBuffer.buffer = (uint8_t *)(0x70000000 + bufferSize);
  
  AudioMemory(128);
  
  control.enable();
  control.inputSelect(AUDIO_INPUT_LINEIN);
  control.volume(0.5);
  
  metronome.length(50);
  metronome.pitchMod(0.8);

  pinMode(timeSignatureButtonPin, INPUT_PULLUP);
  pinMode(looperButtonPin, INPUT_PULLUP);

  beatsPerMinuteKnob.write(beatsPerMinute);

  looperVolume.gain(0.6);

  Serial.begin(115200);
}

void loop() {
  //Read encoder input to adjust BPM
  adjustBPM();

  //Read encoder button to adjust time signature
  adjustTimeSignature();

  //Read looper button to start / stop recording
  adjustRecordingState();

  //If it is time to play a click sound, do so
  playMetronome();

  //If we are recording, move bytes around
  doRecord();

  //If there is something to playback, move bytes around
  doPlayback();
}

void adjustBPM(){
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
}

void adjustTimeSignature(){
  if (timeSignatureButton.update()){
    if (timeSignatureButton.fallingEdge()){
      timeSignatureCount++;
      timeSignatureCount = timeSignatureCount % 9;  //Keep time signature between 0 and 8 inclusive
    }
    Serial.print(timeSignatureCount);
    Serial.println(" Beats");
  }
}

void playMetronome(){
  static uint8_t beatCounter = 0;    //Used to keep state of full notes vs. fractional notes.  We play an emphasised note when this is 0.
  
  if (timeSignatureCount > 0){  //Set time signature to 0 to disable metronome
    uint16_t beatPeriod = (1 / (beatsPerMinute / 60.0) * 1000);
    if (timeSinceLastBeat >= beatPeriod){
      if (state == stop_record){
        stopRecord();
      }
      else if (state == start_record){
        startRecord();
      }
      timeSinceLastBeat -= beatPeriod;
      metronome.frequency(beatCounter == 0 ? 400 : 200);
      metronome.noteOn();
      beatCounter = (beatCounter + 1) % timeSignatureCount;
      if (state == record){
        Serial.print("Recording ");
        Serial.print(recordBuffer.index);
        Serial.println(" bytes");
      }
    }
  }
}

void adjustRecordingState(){
  if (looperButton.update()){
    if (looperButton.fallingEdge()){
      if (state == record){
        if (timeSignatureCount){
          state = stop_record;  //Queue the recording to stop at the next metronome click;
        }
        else {
          stopRecord();
        }
      }
      else if (state == play){
        if (timeSignatureCount){
          state = start_record;   //Queue the recording to start at the next metronome click;
        }
        else {
          startRecord();
        }
      }
    }
  }
}

void startRecord(){
  Serial.println("Recording");
  queueRecord.begin();
  recordBuffer.index = 0;
  state = record;
}

void doRecord(){
  doRecord(0);  //Not in cleanup mode.
}
void doRecord(uint8_t cleanup){
  while(state == record && queueRecord.available() && recordBuffer.index < bufferSize){
    memcpy(recordBuffer.buffer + recordBuffer.index, queueRecord.readBuffer(), 256);
    recordBuffer.index += 256;
    queueRecord.freeBuffer();
  }

  if (!cleanup && recordBuffer.index >= bufferSize){
    stopRecord();
  }
}

void stopRecord(){
  queueRecord.end();
  state = record;
  doRecord(1);   //Finish getting whatever samples are left over
  state = play;
  playBuffer.length = max(playBuffer.length, recordBuffer.index);
  memcpy(playBuffer.buffer, recordBuffer.buffer, recordBuffer.index);
  Serial.print("Finished recording; Playing back ");
  Serial.print(playBuffer.length);
  Serial.println(" bytes");
}

void doPlayback(){
  for (uint8_t i = 0; i < 8 && playBuffer.length > 0 && queuePlay.available(); i++){
    memcpy(queuePlay.getBuffer(), playBuffer.buffer + playBuffer.index, 256);
    queuePlay.playBuffer();
    playBuffer.index += 256;
    if (playBuffer.index >= playBuffer.length){
      playBuffer.index = 0;
      Serial.println("Playing back buffer");
    }
  }
}

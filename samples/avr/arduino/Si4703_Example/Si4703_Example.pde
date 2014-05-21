/*
 1-6-2011
 Spark Fun Electronics 2011
 Nathan Seidle
 
 This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 To use this code, connect the following 5 wires:
 Arduino : Si470x board
 3.3V : VCC
 GND : GND
 A5 : SCLK
 A4 : SDIO
 D2 : RST
 A0 : Trimpot (optional)
 
 Look for serial output at 57600bps.
 
 The Si4703 ACKs the first byte, and NACKs the 2nd byte of a read.
 
 1/18 - after much hacking, I suggest NEVER write to a register without first reading the contents of a chip.
 ie, don't updateRegisters without first readRegisters.
 
 If anyone manages to get this datasheet downloaded
 http://wenku.baidu.com/view/d6f0e6ee5ef7ba0d4a733b61.html
 Please let us know. It seem to be the latest version of the programming guide. It had a change on page 12 (write 0x8100 to 0x07)
 that allowed me to get the chip working..
 
 Also, if you happen to find "AN243: Using RDS/RBDS with the Si4701/03", please share. I love it when companies refer to
 documents that don't exist.
 
 1/20 - Picking up FM stations from a plane flying over Portugal! Sweet! 93.9MHz sounds a little soft for my tastes,s but
 it's in Porteguese.
 
 ToDo:
 Display current status (from 0x0A) - done 1/20/11
 Add RDS decoding - works, sort of
 Volume Up/Down - done 1/20/11
 Mute toggle - done 1/20/11
 Tune Up/Down - done 1/20/11
 Read current channel (0xB0) - done 1/20/11
 Setup for Europe - done 1/20/11
 Seek up/down - done 1/25/11
 
 The Si4703 breakout does work with line out into a stereo or other amplifier. Be sure to test with different length 3.5mm
 cables. Too short of a cable may degrade reception.
 */

#include <Wire.h>

int STATUS_LED = 13;
int resetPin = 2;
int SDIO = A4; //SDA/A4 on Arduino
int SCLK = A5; //SCL/A5 on Arduino
char printBuffer[50];
uint16_t si4703_registers[16]; //There are 16 registers, each 16 bits large

#define FAIL  0
#define SUCCESS  1

#define SI4703 0x10 //0b._001.0000 = I2C address of Si4703 - note that the Wire function assumes non-left-shifted I2C address, not 0b.0010.000W
#define I2C_FAIL_MAX  10 //This is the number of attempts we will try to contact the device before erroring out

//#define IN_EUROPE //Use this define to setup European FM reception. I wuz there for a day during testing (TEI 2011).

#define SEEK_DOWN  0 //Direction used for seeking. Default is down
#define SEEK_UP  1

//Define the register names
#define DEVICEID 0x00
#define CHIPID  0x01
#define POWERCFG  0x02
#define CHANNEL  0x03
#define SYSCONFIG1  0x04
#define SYSCONFIG2  0x05
#define STATUSRSSI  0x0A
#define READCHAN  0x0B
#define RDSA  0x0C
#define RDSB  0x0D
#define RDSC  0x0E
#define RDSD  0x0F

//Register 0x02 - POWERCFG
#define SMUTE  15
#define DMUTE  14
#define SKMODE  10
#define SEEKUP  9
#define SEEK  8

//Register 0x03 - CHANNEL
#define TUNE  15

//Register 0x04 - SYSCONFIG1
#define RDS  12
#define DE  11

//Register 0x05 - SYSCONFIG2
#define SPACE1  5
#define SPACE0  4

//Register 0x0A - STATUSRSSI
#define RDSR  15
#define STC  14
#define SFBL  13
#define AFCRL  12
#define RDSS  11
#define STEREO  8

void setup() {                
  pinMode(13, OUTPUT);
  pinMode(A0, INPUT); //Optional trimpot for analog station control

  Serial.begin(57600);
  Serial.println();

  si4703_init(); //Init the Si4703 - we need to toggle SDIO before Wire.begin takes over.

}

void loop() {
  char option;
  char vol = 15;
  int currentChannel = 977; //Default the unit to a known good local radio station

  gotoChannel(currentChannel);

  while(1) {
    Serial.println();
    Serial.println("Si4703 Configuration");

    currentChannel = readChannel();
    sprintf(printBuffer, "Current station: %02d.%01dMHz", currentChannel / 10, currentChannel % 10);
    Serial.println(printBuffer);

#ifdef IN_EUROPE
    Serial.println("Configured for European Radio");
#endif

    Serial.println("1) Tune to 97.3");
    Serial.println("2) Mute On/Off");
    Serial.println("3) Display status");
    Serial.println("4) Seek up");
    Serial.println("5) Seek down");
    Serial.println("6) Poll for RDS");
    Serial.println("r) Print registers");
    Serial.println("8) Turn GPIO1 High");
    Serial.println("9) Turn GPIO1 Low");
    Serial.println("v) Volume");
    Serial.println("w) Tune up");
    Serial.println("s) Tune down");
    Serial.print(": ");

    while (!Serial.available());
    option = Serial.read();

    if(option == '1')  {
      Serial.println("Tune to 97.3");
      currentChannel = 973;
      //currentChannel = 1023;
      gotoChannel(currentChannel);
    }
    else if(option == '2') {
      Serial.println("Mute toggle");
      si4703_readRegisters();
      si4703_registers[POWERCFG] ^= (1<<DMUTE); //Toggle Mute bit
      si4703_updateRegisters();
    }
    else if(option == '3') {
      si4703_readRegisters();

      Serial.println();
      Serial.println("Radio Status:");

      if(si4703_registers[STATUSRSSI] & (1<<RDSR)){
        Serial.print(" (RDS Available)");

        byte blockerrors = (si4703_registers[STATUSRSSI] & 0x0600) >> 9; //Mask in BLERA
        if(blockerrors == 0) Serial.print (" (No RDS errors)");
        if(blockerrors == 1) Serial.print (" (1-2 RDS errors)");
        if(blockerrors == 2) Serial.print (" (3-5 RDS errors)");
        if(blockerrors == 3) Serial.print (" (6+ RDS errors)");
      }
      else
        Serial.print(" (No RDS)");

      if(si4703_registers[STATUSRSSI] & (1<<STC)) Serial.print(" (Tune Complete)");
      if(si4703_registers[STATUSRSSI] & (1<<SFBL)) 
        Serial.print(" (Seek Fail)");
      else
        Serial.print(" (Seek Successful!)");
      if(si4703_registers[STATUSRSSI] & (1<<AFCRL)) Serial.print(" (AFC/Invalid Channel)");
      if(si4703_registers[STATUSRSSI] & (1<<RDSS)) Serial.print(" (RDS Synch)");

      if(si4703_registers[STATUSRSSI] & (1<<STEREO)) 
        Serial.print(" (Stereo!)");
      else
        Serial.print(" (Mono)");

      byte rssi = si4703_registers[STATUSRSSI] & 0x00FF; //Mask in RSSI
      Serial.print(" (RSSI=");
      Serial.print(rssi, DEC);
      Serial.println(" of 75)");
    }
    else if(option == '4') {
      seek(SEEK_UP);
    }
    else if(option == '5') {
      seek(SEEK_DOWN);
    }
    else if(option == '6') {
      Serial.println("Poll RDS - x to exit");
      while(1) {
        if(Serial.available() > 0)
          if(Serial.read() == 'x') break;

        si4703_readRegisters();
        if(si4703_registers[STATUSRSSI] & (1<<RDSR)){
          Serial.println("We have RDS!");
          byte Ah, Al, Bh, Bl, Ch, Cl, Dh, Dl;
          Ah = (si4703_registers[RDSA] & 0xFF00) >> 8;
          Al = (si4703_registers[RDSA] & 0x00FF);

          Bh = (si4703_registers[RDSB] & 0xFF00) >> 8;
          Bl = (si4703_registers[RDSB] & 0x00FF);

          Ch = (si4703_registers[RDSC] & 0xFF00) >> 8;
          Cl = (si4703_registers[RDSC] & 0x00FF);

          Dh = (si4703_registers[RDSD] & 0xFF00) >> 8;
          Dl = (si4703_registers[RDSD] & 0x00FF);

          Serial.print("RDS: ");
          Serial.print(Ah);
          Serial.print(Al);
          Serial.print(Bh);
          Serial.print(Bl);
          Serial.print(Ch);
          Serial.print(Cl);
          Serial.print(Dh);
          Serial.print(Dl);
          Serial.println(" !");

          delay(40); //Wait for the RDS bit to clear
        }
        else {
          Serial.println("No RDS");
          delay(30); //From AN230, using the polling method 40ms should be sufficient amount of time between checks
        }
        //delay(500);
      }
    }
    else if(option == '8') {
      Serial.println("GPIO1 High");
      si4703_registers[SYSCONFIG1] |= (1<<1) | (1<<0);
      si4703_updateRegisters();
    }
    else if(option == '9') {
      Serial.println("GPIO1 Low");
      si4703_registers[SYSCONFIG1] &= ~(1<<0);
      si4703_updateRegisters();
    }
    else if(option == 'r') {
      Serial.println("Print registers");
      si4703_printRegisters();
    }
    else if(option == 't') {
      Serial.println("Trim pot tuning");
      while(1) {
        if(Serial.available())
          if(Serial.read() == 'x') break;

        int trimpot = 0;
        for(int x = 0 ; x < 16 ; x++)
          trimpot += analogRead(A0);
        trimpot /= 16; //Take average of trimpot reading

        Serial.print("Trim: ");
        Serial.print(trimpot);
        trimpot = map(trimpot, 0, 1023, 875, 1079); //Convert the trimpot value to a valid station
        Serial.print(" station: ");
        Serial.println(trimpot);
        if(trimpot != currentChannel) {
          currentChannel = trimpot;
          gotoChannel(currentChannel);
        }

        delay(100);
      }
    }
    else if(option == 'v') {

      byte current_vol;

      Serial.println();
      Serial.println("Volume:");
      Serial.println("+) Up");
      Serial.println("-) Down");
      Serial.println("x) Exit");
      while(1){
        if(Serial.available()){
          option = Serial.read();
          if(option == '+') {
            si4703_readRegisters(); //Read the current register set
            current_vol = si4703_registers[SYSCONFIG2] & 0x000F; //Read the current volume level
            if(current_vol < 16) current_vol++; //Limit max volume to 0x000F
            si4703_registers[SYSCONFIG2] &= 0xFFF0; //Clear volume bits
            si4703_registers[SYSCONFIG2] |= current_vol; //Set new volume
            si4703_updateRegisters(); //Update
            Serial.print("Volume: ");
            Serial.println(current_vol, DEC);
          }
          if(option == '-') {
            si4703_readRegisters(); //Read the current register set
            current_vol = si4703_registers[SYSCONFIG2] & 0x000F; //Read the current volume level
            if(current_vol > 0) current_vol--; //You can't go lower than zero
            si4703_registers[SYSCONFIG2] &= 0xFFF0; //Clear volume bits
            si4703_registers[SYSCONFIG2] |= current_vol; //Set new volume
            si4703_updateRegisters(); //Update
            Serial.print("Volume: ");
            Serial.println(current_vol, DEC);
          }
          else if(option == 'x') break;
        }
      }
    }

    else if(option == 'w') {
      currentChannel = readChannel();
#ifdef IN_EUROPE
      currentChannel += 1; //Increase channel by 100kHz
#else
      currentChannel += 2; //Increase channel by 200kHz
#endif
      gotoChannel(currentChannel);
    }
    else if(option == 's') {
      currentChannel = readChannel();
#ifdef IN_EUROPE
      currentChannel -= 1; //Decreage channel by 100kHz
#else
      currentChannel -= 2; //Decrease channel by 200kHz
#endif
      gotoChannel(currentChannel);
    }
    else {
      Serial.print("Choice = ");
      Serial.println(option);
    }
  }
}

//Given a channel, tune to it
//Channel is in MHz, so 973 will tune to 97.3MHz
//Note: gotoChannel will go to illegal channels (ie, greater than 110MHz)
//It's left to the user to limit these if necessary
//Actually, during testing the Si4703 seems to be internally limiting it at 87.5. Neat.
void gotoChannel(int newChannel){
  //Freq(MHz) = 0.200(in USA) * Channel + 87.5MHz
  //97.3 = 0.2 * Chan + 87.5
  //9.8 / 0.2 = 49
  newChannel *= 10; //973 * 10 = 9730
  newChannel -= 8750; //9730 - 8750 = 980

#ifdef IN_EUROPE
    newChannel /= 10; //980 / 10 = 98
#else
  newChannel /= 20; //980 / 20 = 49
#endif

  //These steps come from AN230 page 20 rev 0.5
  si4703_readRegisters();
  si4703_registers[CHANNEL] &= 0xFE00; //Clear out the channel bits
  si4703_registers[CHANNEL] |= newChannel; //Mask in the new channel
  si4703_registers[CHANNEL] |= (1<<TUNE); //Set the TUNE bit to start
  si4703_updateRegisters();

  //delay(60); //Wait 60ms - you can use or skip this delay

  //Poll to see if STC is set
  while(1) {
    si4703_readRegisters();
    if( (si4703_registers[STATUSRSSI] & (1<<STC)) != 0) break; //Tuning complete!
    Serial.println("Tuning");
  }

  si4703_readRegisters();
  si4703_registers[CHANNEL] &= ~(1<<TUNE); //Clear the tune after a tune has completed
  si4703_updateRegisters();

  //Wait for the si4703 to clear the STC as well
  while(1) {
    si4703_readRegisters();
    if( (si4703_registers[STATUSRSSI] & (1<<STC)) == 0) break; //Tuning complete!
    Serial.println("Waiting...");
  }
}

//Reads the current channel from READCHAN
//Returns a number like 973 for 97.3MHz
int readChannel(void) {
  si4703_readRegisters();
  int channel = si4703_registers[READCHAN] & 0x03FF; //Mask out everything but the lower 10 bits

#ifdef IN_EUROPE
  //Freq(MHz) = 0.100(in Europe) * Channel + 87.5MHz
  //X = 0.1 * Chan + 87.5
  channel *= 1; //98 * 1 = 98 - I know this line is silly, but it makes the code look uniform
#else
  //Freq(MHz) = 0.200(in USA) * Channel + 87.5MHz
  //X = 0.2 * Chan + 87.5
  channel *= 2; //49 * 2 = 98
#endif

  channel += 875; //98 + 875 = 973
  return(channel);
}

//Seeks out the next available station
//Returns the freq if it made it
//Returns zero if failed
byte seek(byte seekDirection){
  si4703_readRegisters();

  //Set seek mode wrap bit
  //si4703_registers[POWERCFG] |= (1<<SKMODE); //Allow wrap
  si4703_registers[POWERCFG] &= ~(1<<SKMODE); //Disallow wrap - if you disallow wrap, you may want to tune to 87.5 first

  if(seekDirection == SEEK_DOWN) si4703_registers[POWERCFG] &= ~(1<<SEEKUP); //Seek down is the default upon reset
  else si4703_registers[POWERCFG] |= 1<<SEEKUP; //Set the bit to seek up

  si4703_registers[POWERCFG] |= (1<<SEEK); //Start seek

  si4703_updateRegisters(); //Seeking will now start

  //Poll to see if STC is set
  while(1) {
    si4703_readRegisters();
    if((si4703_registers[STATUSRSSI] & (1<<STC)) != 0) break; //Tuning complete!

    Serial.print("Trying station:");
    Serial.println(readChannel());
  }

  si4703_readRegisters();
  int valueSFBL = si4703_registers[STATUSRSSI] & (1<<SFBL); //Store the value of SFBL
  si4703_registers[POWERCFG] &= ~(1<<SEEK); //Clear the seek bit after seek has completed
  si4703_updateRegisters();

  //Wait for the si4703 to clear the STC as well
  while(1) {
    si4703_readRegisters();
    if( (si4703_registers[STATUSRSSI] & (1<<STC)) == 0) break; //Tuning complete!
    Serial.println("Waiting...");
  }

  if(valueSFBL) { //The bit was set indicating we hit a band limit or failed to find a station
    Serial.println("Seek limit hit"); //Hit limit of band during seek
    return(FAIL);
  }

  Serial.println("Seek complete"); //Tuning complete!
  return(SUCCESS);
}

//To get the Si4703 inito 2-wire mode, SEN needs to be high and SDIO needs to be low after a reset
//The breakout board has SEN pulled high, but also has SDIO pulled high. Therefore, after a normal power up
//The Si4703 will be in an unknown state. RST must be controlled
void si4703_init(void) {
  Serial.println("Initializing I2C and Si4703");
  
  pinMode(resetPin, OUTPUT);
  pinMode(SDIO, OUTPUT); //SDIO is connected to A4 for I2C
  digitalWrite(SDIO, LOW); //A low SDIO indicates a 2-wire interface
  digitalWrite(resetPin, LOW); //Put Si4703 into reset
  delay(1); //Some delays while we allow pins to settle
  digitalWrite(resetPin, HIGH); //Bring Si4703 out of reset with SDIO set to low and SEN pulled high with on-board resistor
  delay(1); //Allow Si4703 to come out of reset

  Wire.begin(); //Now that the unit is reset and I2C inteface mode, we need to begin I2C

  si4703_readRegisters(); //Read the current register set
  //si4703_registers[0x07] = 0xBC04; //Enable the oscillator, from AN230 page 9, rev 0.5 (DOES NOT WORK, wtf Silicon Labs datasheet?)
  si4703_registers[0x07] = 0x8100; //Enable the oscillator, from AN230 page 9, rev 0.61 (works)
  si4703_updateRegisters(); //Update

  delay(500); //Wait for clock to settle - from AN230 page 9

  si4703_readRegisters(); //Read the current register set
  si4703_registers[POWERCFG] = 0x4001; //Enable the IC
  //  si4703_registers[POWERCFG] |= (1<<SMUTE) | (1<<DMUTE); //Disable Mute, disable softmute
  si4703_registers[SYSCONFIG1] |= (1<<RDS); //Enable RDS

#ifdef IN_EUROPE
    si4703_registers[SYSCONFIG1] |= (1<<DE); //50kHz Europe setup
  si4703_registers[SYSCONFIG2] |= (1<<SPACE0); //100kHz channel spacing for Europe
#else
  si4703_registers[SYSCONFIG2] &= ~(1<<SPACE1 | 1<<SPACE0) ; //Force 200kHz channel spacing for USA
#endif

  si4703_registers[SYSCONFIG2] &= 0xFFF0; //Clear volume bits
  si4703_registers[SYSCONFIG2] |= 0x0001; //Set volume to lowest
  si4703_updateRegisters(); //Update

  delay(110); //Max powerup time, from datasheet page 13
}

//Write the current 9 control registers (0x02 to 0x07) to the Si4703
//It's a little weird, you don't write an I2C addres
//The Si4703 assumes you are writing to 0x02 first, then increments
byte si4703_updateRegisters(void) {

  Wire.beginTransmission(SI4703);
  //A write command automatically begins with register 0x02 so no need to send a write-to address
  //First we send the 0x02 to 0x07 control registers
  //In general, we should not write to registers 0x08 and 0x09
  for(int regSpot = 0x02 ; regSpot < 0x08 ; regSpot++) {
    byte high_byte = si4703_registers[regSpot] >> 8;
    byte low_byte = si4703_registers[regSpot] & 0x00FF;

    Wire.send(high_byte); //Upper 8 bits
    Wire.send(low_byte); //Lower 8 bits
  }

  //End this transmission
  byte ack = Wire.endTransmission();
  if(ack != 0) { //We have a problem! 
    Serial.print("Write Fail:"); //No ACK!
    Serial.println(ack, DEC); //I2C error: 0 = success, 1 = data too long, 2 = rx NACK on address, 3 = rx NACK on data, 4 = other error
    return(FAIL);
  }

  return(SUCCESS);
}

//Read the entire register control set from 0x00 to 0x0F
void si4703_readRegisters(void){

  //Si4703 begins reading from register upper register of 0x0A and reads to 0x0F, then loops to 0x00.
  Wire.requestFrom(SI4703, 32); //We want to read the entire register set from 0x0A to 0x09 = 32 bytes.

  while(Wire.available() < 32) ; //Wait for 16 words/32 bytes to come back from slave I2C device
  //We may want some time-out error here

  //Remember, register 0x0A comes in first so we have to shuffle the array around a bit
  for(int x = 0x0A ; ; x++) { //Read in these 32 bytes
    if(x == 0x10) x = 0; //Loop back to zero
    si4703_registers[x] = Wire.receive() << 8;
    si4703_registers[x] |= Wire.receive();
    if(x == 0x09) break; //We're done!
  }
}

void si4703_printRegisters(void) {
  //Read back the registers
  si4703_readRegisters();

  //Print the response array for debugging
  for(int x = 0 ; x < 16 ; x++) {
    sprintf(printBuffer, "Reg 0x%02X = 0x%04X", x, si4703_registers[x]);
    Serial.println(printBuffer);
  }
}














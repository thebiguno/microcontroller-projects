/**
 *
 * Sample I2C implementation.  Sends a button state over I2C to a slave Arduino
 * with an LED flashing showing button state.
 * 
 * Connections: Arduino analog pins 4 and 5 are connected between the two Arduinos, 
 * with a 1k pullup resistor connected to each line.  On the Master, connect a push 
 * button between digital pin 10 and ground.  On the slave, connect an LED (with a
 * resistor) to digital pin 9.
 * 
 */

#include <Wire.h>

#define BUTTON 10
#define ADDRESS 0x4C

byte data = 0x55;

void setup() {
  Serial.begin(115200);           // start serial for output
  pinMode(BUTTON, INPUT);
  digitalWrite(BUTTON, HIGH);
  Wire.begin();
  
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  
  Wire.beginTransmission(ADDRESS);
  Wire.send(0x07);
  Wire.send(0x01);
  Wire.endTransmission();  
}

boolean last_state = HIGH;
void loop() {
  if (digitalRead(BUTTON) != last_state){
    last_state = digitalRead(BUTTON);
    Serial.write("Ended register transmission\n");

    Wire.beginTransmission(ADDRESS);
    Wire.send(0x00);
    Wire.endTransmission();  

    Wire.requestFrom(ADDRESS, 10);
    while (Wire.available()) {
      data = Wire.receive();
      Serial.write("Received ");
      Serial.print(data, HEX);
      Serial.write("\n");
    }
//    else {
//      data = 0x1;
//      Serial.write("Could not receive! ");
//    }
    
  }
} 

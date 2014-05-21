/*
 * Shift Register example.  Displays a binary representation of numbers on a set
 * of 8 LEDs, connected to a shift register.
 *
 * To wire this example, you need a 74595 shift register / latch, 8 LEDs / resistors,
 * and a button.  Wire the button to ground (there is an internal pullup to keep it
 * HIGH when not pressed).  Connect the DATA_PIN to pin 14 on the shift register;
 * CLOCK_PIN goes to pin 11 on the shift register; LATCH_PIN goes to pin 12.
 */
 
#define DATA_PIN 13
#define CLOCK_PIN 12
#define LATCH_PIN 11

#define BUTTON_PIN 10

boolean buttonState = HIGH;
byte value = 0;

void setup(){
  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, HIGH);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, HIGH);
  
  Serial.begin(115200);
}

void loop(){
  
  if (digitalRead(BUTTON_PIN) != buttonState){
    Serial.println("Foo");
    buttonState = !buttonState;
    shift(value++);     
  }
}

void shift(byte data){
  //Low latch enabled writing
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, data);
  //Once the latch pin goes high the contents of the 
  // shift register are transferred to output.
  digitalWrite(LATCH_PIN, HIGH);
  Serial.println(data, HEX);
}

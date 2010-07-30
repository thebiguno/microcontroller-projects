/*
 * Adjustable Slave Flash, to be used with old manual flashes.
 *
 * This device lets you trigger multiple slave flashes after a specified delay.
 * The delay is camera dependent, depending on TTL Preflash, redeye mode, and
 * numerous other considerations.  Mostly it will be found by trial and error;
 * however, some rough guidelines as found through my testing are included here.
 *
 * All these values are for an Olympus E-520 DSLR camera
 * 
 * 0x5A ms - RC flash mode OFF, built-in popup flash only
 * 0x74 ms - RC flash mode; 0 TTL flashes
 * 0x7E ms - RC flash mode; 1 TTL flash
 * 0x88 ms - RC flash mode; 2 TTL flashes
 * 0x92 ms  - RC flash mode; 3 TTL flashes
 *
 * (c) Wyatt Olson 2009
 */

#include <EEPROM.h>

//Set to 1 to debug
#define DEBUG 0

//Beginning LED data pin
#define D_START 2
//Total count of data pins
#define D_NUM 7

//The beginning LED control pin
#define C_START 9
//Total count of control pins; max is C_START + (C_NUM - 1)
#define C_NUM 3

//Two buttons: down and up
#define BUT_UP 15
#define BUT_DOWN 14

//Triggers the flash via TRIAC
#define FLASH_TRIGGER 19

//Analog light sensor
#define LIGHT 2

//How much instantaneous change required to trigger flash.
#define LIGHT_THRESHOLD 10

//How long the trigger pulse should be
//#define FLASH_LENGTH 50

//Delay range
#define DELAY_MAX 300
#define DELAY_MIN 0

//Delay interval when holding buttons down
#define QUICK_BUTTONS 50

//Debounce settings
#define BUT_DEBOUNCE 200


//Map between values and segments on the 7 segment LED.
//This is the inverse of the segments needed; i.e., enable 
// a segment by setting this to a 0, disable with a 1.
boolean segments[][D_NUM] = 
{
  {
    0, 0, 0, 0, 0, 0, 1  }
  , //0
  {
    1, 0, 0, 1, 1, 1, 1  }
  , //1
  {
    0, 0, 1, 0, 0, 1, 0  }
  , //2
  {
    0, 0, 0, 0, 1, 1, 0  }
  , //3
  {
    1, 0, 0, 1, 1, 0, 0  }
  , //4
  {
    0, 1, 0, 0, 1, 0, 0  }
  , //5
  {
    0, 1, 0, 0, 0, 0, 0  }
  , //6
  {
    0, 0, 0, 1, 1, 1, 1  }
  , //7
  {
    0, 0, 0, 0, 0, 0, 0  }
  , //8
  {
    0, 0, 0, 0, 1, 0, 0  }
  , //9
  {
    0, 0, 0, 1, 0, 0, 0  }
  , //A
  {
    1, 1, 0, 0, 0, 0, 0  }
  , //B
  {
    0, 1, 1, 0, 0, 0, 1  }
  , //C
  {
    1, 0, 0, 0, 0, 1, 0  }
  , //D
  {
    0, 1, 1, 0, 0, 0, 0  }
  , //E
  {
    0, 1, 1, 1, 0, 0, 0  }
  , //F
  {
    1, 1, 1, 1, 1, 1, 1  } //Blank
};

int digit_counter = 0; //Remember which LED digit we last displayed
boolean show_flash_delay = true;  //Only show display when true
int digit = 0;

long last_increment = 0, last_digit_counter_increment = 0;



long but_up_last_press = 0, but_down_last_press = 0, but_last_press = 0, temp_long;
int but_down_value, but_up_value, but_up_last_value, but_down_last_value, i;

//Flash delay value, stored in EEPROM
int flash_delay;
//Do we need to write the current value to EEPROM?
boolean flash_delay_dirty = false; 
#define FLASH_DELAY_WRITE_TIME 5000
#define EEPROM_MSB 0x0
#define EEPROM_LSB 0x1


//Light sensor variable
int light_value, light_last_value = 0;


void setup() {
  //Debugging
  if (DEBUG)
    Serial.begin(115200);

  //Set up the trigger
  pinMode(FLASH_TRIGGER, OUTPUT);
  digitalWrite(FLASH_TRIGGER, LOW);

  //Set up LEDs
  for (i = 0; i < D_NUM; i++)
    pinMode(D_START + i, OUTPUT);  

  //Set up LED control pins
  for (i = 0; i < C_NUM; i++)
    pinMode(C_START + i, OUTPUT);

  //Set up buttons, w/ pullup resistors
  pinMode(BUT_UP, INPUT);
  pinMode(BUT_DOWN, INPUT);
  digitalWrite(BUT_UP, HIGH);
  digitalWrite(BUT_DOWN, HIGH);

  //Read the current flash delay from EEPROM.  Stored in two bytes.
  flash_delay = (EEPROM.read(EEPROM_MSB) << 0x8) | EEPROM.read(EEPROM_LSB);
  //Sanity checks on flash delay
  if (flash_delay < DELAY_MIN)
    flash_delay = DELAY_MIN;
  else if (flash_delay > DELAY_MAX)
    flash_delay = DELAY_MAX;
  flash_delay_dirty = true;

  //Initialize the flash so as not to always flash initially  
  light_last_value = analogRead(LIGHT);
  but_up_last_value = digitalRead(BUT_UP);
  but_down_last_value = digitalRead(BUT_DOWN);
}

void loop() {
  //Ensure flash is low
  digitalWrite(FLASH_TRIGGER, LOW);

  //Read all the buttons / sensor
  light_value = analogRead(LIGHT);
  but_up_value = digitalRead(BUT_UP);
  but_down_value = digitalRead(BUT_DOWN);

  //Trigger the flash when the light rises quickly, not when it falls
  if (light_value - LIGHT_THRESHOLD > light_last_value){
    delay(flash_delay);
    digitalWrite(FLASH_TRIGGER, HIGH);
    if (DEBUG){
      Serial.print("Firing flash: light_last_value = ");
      Serial.print(light_last_value);
      Serial.print(", light_value = ");
      Serial.print(light_value);
      Serial.println(".");
    }
    //    delay(FLASH_LENGTH);
    //    digitalWrite(FLASH_TRIGGER, LOW);
  }

  //Check for button presses
  if (but_up_value == LOW && millis() - but_up_last_press > BUT_DEBOUNCE){
    if (show_flash_delay){
      flash_delay++;

      if (flash_delay > DELAY_MAX)
        flash_delay = DELAY_MAX;

      if (but_up_last_value == LOW){
        temp_long = millis();
        while (millis() - temp_long < QUICK_BUTTONS)
          display(flash_delay);
      }
      else
        but_up_last_press = millis();

      but_last_press = millis();
      flash_delay_dirty = true;

      if (DEBUG)
        Serial.println(flash_delay);
    }
    else {
      show_flash_delay = true; 
      but_up_last_press = millis();
      flash_delay_dirty = true;
      but_last_press = millis();
    }
  }

  if (but_down_value == LOW && millis() - but_down_last_press > BUT_DEBOUNCE){
    if (show_flash_delay){
      flash_delay--;

      if (flash_delay < DELAY_MIN)
        flash_delay = DELAY_MIN;

      if (but_down_last_value == LOW){
        temp_long = millis();
        while (millis() - temp_long < QUICK_BUTTONS)
          display(flash_delay);
      }
      else 
        but_down_last_press = millis();

      but_last_press = millis();
      flash_delay_dirty = true;

      if (DEBUG)
        Serial.println(flash_delay);
    }
    else {
      show_flash_delay = true; 
      but_down_last_press = millis();
      flash_delay_dirty = true;
      but_last_press = millis();
    }
  }

  //After a few seconds delay, write the current flash delay to EEPROM
  if (flash_delay_dirty && millis() - but_last_press > FLASH_DELAY_WRITE_TIME){
    flash_delay_dirty = false;
    EEPROM.write(EEPROM_MSB, flash_delay >> 0x8);
    EEPROM.write(EEPROM_LSB, flash_delay & 0xFF);
    if (DEBUG){
      Serial.print("Saved value ");
      Serial.print(flash_delay, DEC);
      Serial.print(" to EEPROM; verified as ");
      Serial.println((EEPROM.read(EEPROM_MSB) << 0x8) | EEPROM.read(EEPROM_LSB), DEC);
    }
    
    show_flash_delay = false; //Turn off display once value saved.
    displayLED(0, -1);
    displayLED(1, -1);
    displayLED(2, -1);
  }

  //Remember the last read values
  light_last_value = light_value;
  but_up_last_value = but_up_value;
  but_down_last_value = but_down_value;


  //Refresh the display  
  if (show_flash_delay)
    display(flash_delay);
}


void display(int value){
  //Verify that the value is within a valid range.  
  // Currently hardcoded for hex for convenience, but we could 
  // use others if desired.
  if (value < pow(0x10, C_NUM) && value > 0){
    //Determine which digit is going to be displayed
    displayLED(digit_counter, (unsigned int) value >> (digit_counter * 4) & 0xF);
  }
  if (millis() - last_digit_counter_increment > 5){
    digit_counter = (digit_counter + 1) % C_NUM;
    last_digit_counter_increment = millis();
  }
}

//Value is the value to show, digit is the LED segment number to enable.
void displayLED(int digit, int value){
  if (value > 0x10 || value < 0x0)
    displayLED(digit, 0x10); //Blank LED if we got an invalid value
  else if (digit <= C_NUM){
    for (i = 0; i < C_NUM; i++)
      digitalWrite(C_START + i, (digit == i));  //Enable digit, and disable all the rest

    for (i = 0; i < D_NUM; i++)
      digitalWrite(D_START + i, segments[value][i]);
  }
}



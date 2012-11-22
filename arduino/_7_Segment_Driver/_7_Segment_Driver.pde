//Beginning data pin
#define D_START 2
//Total count of data pins
#define D_NUM 7

//The beginning control pin
#define C_START 9
//Total count of control pins; max is C_START + (C_NUM - 1)
#define C_NUM 3

//Map between values and segments on the 7 segment LED.
//This is the inverse of the segments needed; i.e., enable 
// a segment by setting this to a 0, disable with a 1.
boolean segments[][D_NUM] = 
{
  {0, 0, 0, 0, 0, 0, 1}, //0
  {1, 0, 0, 1, 1, 1, 1}, //1
  {0, 0, 1, 0, 0, 1, 0}, //2
  {0, 0, 0, 0, 1, 1, 0}, //3
  {1, 0, 0, 1, 1, 0, 0}, //4
  {0, 1, 0, 0, 1, 0, 0}, //5
  {0, 1, 0, 0, 0, 0, 0}, //6
  {0, 0, 0, 1, 1, 1, 1}, //7
  {0, 0, 0, 0, 0, 0, 0}, //8
  {0, 0, 0, 0, 1, 0, 0}, //9
  {0, 0, 0, 1, 0, 0, 0}, //A
  {1, 1, 0, 0, 0, 0, 0}, //B
  {0, 1, 1, 0, 0, 0, 1}, //C
  {1, 0, 0, 0, 0, 1, 0}, //D
  {0, 1, 1, 0, 0, 0, 0}, //E
  {0, 1, 1, 1, 0, 0, 0}, //F
  {1, 1, 1, 1, 1, 1, 1} //Blank
};

int digit_counter = 0; //Remember which digit we last displayed
int count = 0;
int i;
int digit = 0;

long last_increment = 0, last_digit_counter_increment = 0;

void setup(){
  for (i = 0; i < D_NUM; i++)
    pinMode(D_START + i, OUTPUT);
}

void loop(){
  display(count);
  
  if (millis() - last_increment > 250){
    count++;
    last_increment = millis();
  }
  
  count = count % 0xFF;
}

void display(int value){
  //Verify that the value is within a valid range.  
  // Currently hardcoded for hex for convenience, but we could 
  // use others if desired.
  if (value < pow(0x10, C_NUM) && value > 0){
    //Determine which digit is going to be displayed
    displayLED(digit_counter, (unsigned int) value >> (digit_counter * 4) & 0xF);
  }
  if (millis() - last_digit_counter_increment > 1){
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



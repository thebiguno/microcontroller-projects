/*
Drum Master - controller for up to 64 drum sensors.  
Copyright 2008 Wyatt Olson <wyatt.olson@gmail.com>


*/

#define A_IN 0
#define D_IN 8

//Multiplexer selector pins.  S5 is MSB; S0 is LSB.
#define S5 7
#define S4 6
#define S3 5
#define S2 4
#define S1 3
#define S0 2

//This should now be done in hardware via transistor, reporting digital hits.
//#define PIEZO_THRESHOLD 50  //Min value to report
#define ANALOG_WINDOW 20   //How long to read a single analog hit (ms).
#define BOUNCE_WINDOW 250  //After a hit, don't report the same sensor for this long (ms).

int i, val = 100, tmp;

long last_read_time[64];

void setup() {
  pinMode(D_IN, INPUT);
  
  pinMode(S5, OUTPUT);
  pinMode(S4, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S0, OUTPUT);
  
  Serial.begin(115200);
  Serial.println("Starting...");
}

void loop() {

  for (i = 0; i <= 0x3F; i++){
    select(i);
    if (!digitalRead(D_IN) && millis() - last_read_time[i] > BOUNCE_WINDOW){
      if (millis() - last_read_time[i] > BOUNCE_WINDOW){
        val = getVelocity();
//        if (val > PIEZO_THRESHOLD){
          Serial.println(val);
          last_read_time[i] = millis();
//        }
      }    
    }
  }
}

/*
 * Sets pins S5 - S0 to select the multiplexer output.
 */
void select(int s){
  digitalWrite(S5, s & 0x20 != 0);
  digitalWrite(S4, s & 0x10 != 0);
  digitalWrite(S3, s & 0x8 != 0);
  digitalWrite(S2, s & 0x4 != 0);
  digitalWrite(S1, s & 0x2 != 0);
  digitalWrite(S0, s & 0x1 != 0);
}

/* 
 * Returns the velocity of an analog piezo strike.  Currently returns the largest
 * value obtained by reading over the ANALOG_WINDOW period; a possible enhancement 
 * to this is to use some weighted average of a few (2? 3? 4?) hardest strikes.
 */
int getVelocity(){
  int max_val = 0;
  long start = millis();
  while (millis() - start < ANALOG_WINDOW){
     val = analogRead(A_IN);
     if (val > max_val)
       max_val = val;
  }
  
  return max_val;
}

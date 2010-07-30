#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define A_IN 0
#define A_TRIGGER 15
#define THRESHOLD 15

int last_hit = 0;
int v, x;

void setup() {
  cbi(ADMUX, REFS1);   //set a2d reference voltage to external AREF pin ...
  cbi(ADMUX, REFS0);   //... so that ADC uses 3V instead of 5V
  
  Serial.begin(115200);
}

void loop() {
  v = analogRead(A_IN);
  if (v > THRESHOLD){
    if (millis() - last_hit > 500)
      last_hit = millis();
    x = millis() - last_hit;
    Serial.print(x);
    Serial.print(";");    
    Serial.println(v);
  }
}

#define A_IN 0
#define D_TRIGGER 8
#define THRESHOLD 10
#define LENGTH 500

int del = 90;

int v, last_value = 0;
long time, first_time = 0, m, last_flash;

void setup() {
  Serial.begin(115200);
  pinMode(D_TRIGGER, OUTPUT);
  digitalWrite(D_TRIGGER, LOW);
  last_value = analogRead(A_IN);
  time = 0;
}

void loop() {
  v = analogRead(A_IN);
  
  m = millis();
  
  if ((v + THRESHOLD) < last_value || (v - THRESHOLD) > last_value){
    time = m;
//    if (first_time = 0)
//      first_time = millis();
  }
  
  if (time != 0 && m - time < LENGTH && m - last_flash > LENGTH){
//    Serial.print((m - first_time));
//    Serial.print(",");
//    Serial.println(v);
//    last_value = v;
//    delay(90); //RC flash mode OFF
//    delay(116); //RC flash mode; 0 TTL flash
//    delay(126); //RC flash mode; 1 TTL flash
    delay(136); //RC flash mode; 2 TTL flash
//    delay(146); //RC flash mode; 3 TTL flash
//    delay(del);
    del = del + 1;
    digitalWrite(D_TRIGGER, HIGH);
    Serial.println(del);
    last_flash = m;
  }
  else if (m - time >= LENGTH) {
     digitalWrite(D_TRIGGER, LOW);
  }
}

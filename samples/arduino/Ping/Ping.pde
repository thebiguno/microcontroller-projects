/*
Drum Master - controller for up to 64 drum sensors.  
Copyright 2008 Wyatt Olson <wyatt.olson@gmail.com>
*/

long start, time;
int s, b;

void setup() {
  Serial.begin(115200);
  Serial.println("Initialized");
}

void loop() {
  delay(random(2000));

  start = millis();  
  s = random(0, 10);
  Serial.println(s);
  while (!Serial.available())
    delayMicroseconds(100);
  b = Serial.read();
  time = millis() - start;
  Serial.print(time);
  Serial.print(":");
  Serial.print(s);
  Serial.print(":");
  Serial.println(b);
}

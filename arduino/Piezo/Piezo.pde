#define LED 9
#define BUZZER 10

int val = 0;

void setup(){
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
}

void loop(){
  analogWrite(BUZZER, val);
  
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  delay(500);
  
  val++;
  if (val >= 255)
    val = 0;
}

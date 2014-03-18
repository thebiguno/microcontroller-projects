#define RED 2
#define WHITE 3
#define BLUE 4

void setup() {
  //We put the pins in input mode because we are going
  // to be using them for both input and (data) output.
  // We ground them all via ~100k resistors.
  pinMode(RED, INPUT); 
  pinMode(WHITE, INPUT);
  pinMode(BLUE, INPUT);

  //Pull blue low; camera will set it high on startup
  pinMode(BLUE, LOW);

  //Pull white low  
  pinMode(WHITE, LOW);
  pinMode(RED, HIGH);
  delay(16); //Not sure if we do the delay, or if the camera turns it low again...
  pinMode(RED, LOW);
}

void loop(){
  if (digitalRead(WHITE) == HIGH){
    listenInitPacket();
  }
}

void listenInitPacket(){
  int count = 0;
  boolean state = HIGH;
  while(count < 7){
    if (digitalRead(WHITE) == HIGH){
      state = HIGH;
    }
    else if (state == HIGH){ //we have switched from high to low
       count++;
    }
  } 
  
  //We wait for 741 microseconds after the falling pulse of 
  delayMicroseconds(741);
  
  sendFlashPacket();
}

void sendFlashPacket(){
  //TODO 
}

void listenCameraPacket(){
  
}

void sendAckPacket(){
   
}

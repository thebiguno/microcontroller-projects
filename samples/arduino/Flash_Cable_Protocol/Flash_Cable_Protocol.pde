#define BUFFER_SIZE 120

long times[BUFFER_SIZE];
char states[BUFFER_SIZE];

//head / tail index pointers for circular buffer.  We write to head and read from tail.
volatile int head = 0, tail = 0;

void setup() {
  //Enable pin change interrupts for pins 23::26 (first 4 pins on change interrupt 1; maps to lower 4 bits of PORTC)
  PCMSK1 = (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT10) | (1 << PCINT11);

  //Enable Pin Change Interrupt 1
  PCICR = (1 << PCIE1);
	
  //Set Port C to Input mode
  PORTC = 0x0;
  DDRC = 0x0;
	
  //Set pin 14 (Port B 0) to output mode
  DDRB = (1 << DDB0);
  
  Serial.begin(115200);
}

void loop() {
  if (head != tail){
    Serial.print((times[tail] >> 24) & 0xFF);			
    Serial.print((times[tail] >> 16) & 0xFF);
    Serial.print((times[tail] >> 8) & 0xFF);
    Serial.print(times[tail] & 0xFF);

    Serial.print(states[tail]);

    tail++;
    if (tail >= BUFFER_SIZE) tail = 0;
  }
}

ISR(PCINT1_vect){
  //Read the data bits into state, and record micros
  if (head + 1 == tail || (head + 1 == BUFFER_SIZE && tail == 0)){
    //If there is a buffer overrun, light the LED.
    PORTB = (1 << PORTB0);
  } else {
    times[head] = micros();
    states[head] = PINC & 0xF;
    head++;
    if (head >= BUFFER_SIZE) head = 0;

    //Once the buffer is cleared, clear the error
    PORTB = 0x0;
  }
}

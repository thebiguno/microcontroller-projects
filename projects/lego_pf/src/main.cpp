#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#ifdef DEBUG
#include <SerialAVR.h>
using namespace digitalcave;
#endif

//What channel to listen for
#define CHANNEL				0

#define MOTOR_RED_PORT		PORTD
#define MOTOR_RED_DDR		DDRD
//Arduino pin 2
#define MOTOR_RED_PWM		PORTD2
//Arduino pin 3
#define MOTOR_RED_A			PORTD3
//Arduino pin 4
#define MOTOR_RED_B			PORTD4

#define MOTOR_BLUE_PORT		PORTC
#define MOTOR_BLUE_DDR		DDRC
//Arduino pin A1
#define MOTOR_BLUE_PWM		PORTC1
//Arduino pin A2
#define MOTOR_BLUE_A		PORTC2
//Arduino pin A3
#define MOTOR_BLUE_B		PORTC3

#define SERVO_PORT			PORTC
#define SERVO_DDR			DDRC
#define SERVO_PIN			PORTC0

//Pin that the LED is connected to
#define LED_PORT			PORTB
#define LED_DDR			DDRB
#define LED_PIN				PORTB5

//Pin that the IR sensor is connected to
#define SENSOR_PORT			PINB
#define SENSOR_PIN			PORTB1

//See http://www.philohome.com/pf/LEGO_Power_Functions_RC.pdf page 6 for protocol packet description
typedef struct message {
	uint8_t toggle : 1;				//Toggle bit (0 / 1)
	uint8_t escape : 1;				//Escape bit (0 / 1)
	uint8_t channel : 2;			//Channel (0 - 3)
	uint8_t address : 1;			//Address
	uint8_t mode : 3;				//Mode (see docs)
	uint8_t data : 4;				//Data (mode dependent, see docs)
	uint8_t new_mail : 1;			//Is this message new?  1 = yes, 0 = no.
} message_t;

static volatile uint16_t raw_message = 0;
static volatile uint8_t mailbox_new = 0;
static volatile message_t message;

//Can be from -7 to 7 inclusive
static volatile int8_t motor_red_speed;
static volatile int8_t motor_blue_speed;

int main() __attribute__((noreturn));
int main(){
	//Enable motor pins as output
	MOTOR_RED_DDR |= _BV(MOTOR_RED_PWM) | _BV(MOTOR_RED_A) | _BV(MOTOR_RED_B);
	MOTOR_BLUE_DDR |= _BV(MOTOR_BLUE_PWM) | _BV(MOTOR_BLUE_A) | _BV(MOTOR_BLUE_B);

	//Enable servo pin
	SERVO_DDR |= _BV(SERVO_PIN);

	//Enable LED
	LED_DDR |= _BV(LED_PIN);

	//Enable pin change interrupt
	PCICR |= _BV(PCIE0);														//Enable pin change interrupts for encoders
	PCMSK0 |= _BV(PCINT1);														//Enable bit 1 for pin change interrupts

	//Set up TIMER0 for motor PWM
	TCCR0A = 0x00;	//Normal mode
	TCCR0B = _BV(CS02) | _BV(CS00);		//CLK div 1024
	//Enable both compares and overflow interrupts.  We turn on the motor at overflow, and turn it off in the individual compare matches
	TIMSK0 = _BV(OCIE0B) | _BV(OCIE0A) | _BV(TOIE0);

	//Set up TIMER1 for reading sensor
	TCCR1A = 0x0;
	//Enable overflow interrupt - this signals timeout.
	TIMSK1 = _BV(TOIE1);

	//Set up TIMER2 for servo PWM
	TCCR2A = _BV(WGM20);		//PWM Phase Correct mode, /1024 prescaler
	TCCR2B = _BV(WGM22) | _BV(CS22) | _BV(CS21) | _BV(CS20);
	//Enable both compare interrupts.  We reset the counter and turn the pin high on A, and then turn the pin low on B.
	TIMSK2 = _BV(OCIE2B) | _BV(OCIE2A);
	OCR2A = 0xFF;

	sei();

#ifdef DEBUG
	SerialAVR serial(9600);
	char buffer[100];
#endif

	//Main program loop
	while (1){
		if (message.new_mail){
			message.new_mail = 0;
			if (message.channel == CHANNEL){					//Check the channel
				if (message.address == 0){						//Current address space
					if ((message.mode & 0x04) == 0x04){			//Single output mode
						if ((message.mode & 0x02) == 0x02){		//Clear / Set / Toggle pin
							if (message.data == 0x04){			//Increment PWM
								if ((message.mode & 0x01) == 0x01){
									motor_blue_speed++;
									if (motor_blue_speed > 7){
										motor_blue_speed = 7;
									}
								}
								else {
									motor_red_speed++;
									if (motor_red_speed > 7){
										motor_red_speed = 7;
									}
									MOTOR_RED_PORT &= ~(_BV(MOTOR_RED_PWM) | _BV(MOTOR_RED_A));
									MOTOR_RED_PORT |= _BV(MOTOR_RED_B);
								}
							}
							else if (message.data == 0x05){		//Decrement PWM
								if ((message.mode & 0x01) == 0x01){
									motor_blue_speed--;
									if (motor_blue_speed < -7){
										motor_blue_speed = -7;
									}
									MOTOR_BLUE_PORT &= ~(_BV(MOTOR_BLUE_PWM) | _BV(MOTOR_BLUE_B));
									MOTOR_BLUE_PORT |= _BV(MOTOR_BLUE_A);
								}
								else {
									motor_red_speed--;
									if (motor_red_speed < -7){
										motor_red_speed = -7;
									}
									MOTOR_RED_PORT &= ~(_BV(MOTOR_RED_PWM) | _BV(MOTOR_RED_B));
									MOTOR_RED_PORT |= _BV(MOTOR_RED_A);
								}
							}
						}
						else {									//PWM
							if (message.data == 0x08){			//Brake
								if ((message.mode & 0x01) == 0x01){
									motor_blue_speed = 0;
									MOTOR_BLUE_PORT &= ~(_BV(MOTOR_BLUE_PWM));
									MOTOR_BLUE_PORT |= _BV(MOTOR_BLUE_A) | _BV(MOTOR_BLUE_B);
								}
								else {
									motor_red_speed = 0;
									MOTOR_RED_PORT &= ~(_BV(MOTOR_RED_PWM));
									MOTOR_RED_PORT |= _BV(MOTOR_RED_A) | _BV(MOTOR_RED_B);
								}
							}
						}
					}
				}
			}

			if (motor_red_speed > 0){
				MOTOR_RED_PORT &= ~_BV(MOTOR_RED_A);
				MOTOR_RED_PORT |= _BV(MOTOR_RED_B);
				OCR0A = motor_red_speed << 5;
			}
			else if (motor_red_speed < 0){
				MOTOR_RED_PORT &= ~_BV(MOTOR_RED_B);
				MOTOR_RED_PORT |= _BV(MOTOR_RED_A);
				OCR0A = (motor_red_speed * -1) << 5;
			}
			else {
				MOTOR_RED_PORT |= _BV(MOTOR_RED_A) | _BV(MOTOR_RED_B);
			}

			if (motor_blue_speed > 0){
				MOTOR_BLUE_PORT &= ~_BV(MOTOR_BLUE_A);
				MOTOR_BLUE_PORT |= _BV(MOTOR_BLUE_B);
				OCR0B = motor_blue_speed << 5;
			}
			else if (motor_blue_speed < 0){
				MOTOR_BLUE_PORT &= ~_BV(MOTOR_BLUE_B);
				MOTOR_BLUE_PORT |= _BV(MOTOR_BLUE_A);
				OCR0B = (motor_blue_speed * -1) << 5;
			}
			else {
				MOTOR_BLUE_PORT |= _BV(MOTOR_BLUE_A) | _BV(MOTOR_BLUE_B);
			}

			MOTOR_RED_PORT |= _BV(MOTOR_RED_PWM);
			MOTOR_BLUE_PORT |= _BV(MOTOR_BLUE_PWM);

			OCR2B = (motor_blue_speed * 2) + 22;

#ifdef DEBUG
			serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "message: [t:%d e:%d c:%d a:%d m:0x%02X d:0x%02X]\n\r", message.toggle, message.escape, message.channel, message.address, message.mode, message.data));
			serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "red: %d, blue: %d\n\r", motor_red_speed, motor_blue_speed));
#endif
		}
	}
}

ISR(PCINT0_vect){
	if (!(SENSOR_PORT & _BV(SENSOR_PIN))){			//If this is a low reading, it means we are starting a new bit.  Record the last one, and reset the timer.
		TCCR1B = _BV(CS10);		//Start timer running at F_CPU in Normal mode.
		uint16_t micros = TCNT1 / (F_CPU / 1000000);		//Count micros since last time.  //Every (F_CPU / 1000000) clock ticks is one microsecond.  This is 16 at 16MHz.
		TCNT1 = 0;				//Reset timer counter

		//See http://www.philohome.com/pf/LEGO_Power_Functions_RC.pdf page 14 for protocol timing documentation
		if (micros >= 316 && micros < 526){
			raw_message = raw_message << 1;	//Shift in a zero bit
		}
		else if (micros >= 526 && micros < 947){
			raw_message = raw_message << 1 | 0x01;		//Shift in a one bit
		}
		else if (micros >= 947 && micros < 1579){
			raw_message = 0;		//Start bit
			LED_PORT |= _BV(LED_PIN);
		}
	}
}

ISR(TIMER0_OVF_vect){
	if (motor_red_speed != 0){
		MOTOR_RED_PORT |= _BV(MOTOR_RED_PWM);
	}
	if (motor_blue_speed != 0){
		MOTOR_BLUE_PORT |= _BV(MOTOR_BLUE_PWM);
	}
}

ISR(TIMER0_COMPA_vect){
	MOTOR_RED_PORT &= ~_BV(MOTOR_RED_PWM);
}

ISR(TIMER0_COMPB_vect){
	MOTOR_BLUE_PORT &= ~_BV(MOTOR_BLUE_PWM);
}

ISR(TIMER1_OVF_vect){
	//Timeout and process raw_message. We overflow when the timer goes to MAX (0xFFFF).  This is 4.096 ms after the last incoming bit; a bit out of spec, but not too bad.
	TCCR1B = 0x00;		//Stop timer

	LED_PORT &= ~_BV(LED_PIN);

	if (((raw_message >> 15) & 0x01) != message.toggle){	//If the toggle bit is the same, we ignore the message - it is a repeat of the last one (the transmitter sends 4 messages for redundancy)
		uint8_t message_checksum = (raw_message >> 0) & 0x0F;
		uint8_t calculated_checksum = 0x0F ^ ((raw_message >> 12) & 0x0F) ^ ((raw_message >> 8) & 0x0F) ^ ((raw_message >> 4) & 0x0F);
		if (message_checksum == calculated_checksum){	//Check the checksum.  If it does not match, we discard the message.
			message.toggle = (raw_message >> 15) & 0x01;
			message.escape = (raw_message >> 14) & 0x01;
			message.channel = (raw_message >> 12) & 0x03;
			message.address = (raw_message >> 11) & 0x01;
			message.mode = (raw_message >> 8) & 0x07;
			message.data = (raw_message >> 4) & 0x0F;
			message.new_mail = 1;
		}
	}

	raw_message = 0;
}

ISR(TIMER2_COMPA_vect){
	//Turn on the servo pin and restart the timer counter
	SERVO_PORT |= _BV(SERVO_PIN);
	TCNT2 = 0;
}

ISR(TIMER2_COMPB_vect){
	//Turn off the servo pin
	SERVO_PORT &= ~_BV(SERVO_PIN);
}

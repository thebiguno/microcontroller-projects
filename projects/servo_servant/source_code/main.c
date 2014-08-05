//------------------------------------------------------------------------------
/*
 * Servo Slave
 */
//------------------------------------------------------------------------------

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//------------------------------------------------------------------------------
// Pin and Setting Definitions
//------------------------------------------------------------------------------

#define LEDDDR DDRB
#define LEDPORT PORTB
#define LEDPORTA PORTB0
#define LEDPORTB PORTB1
#define LEDPORTC PORTB2
#define LEDPORTD PORTB3
#define LEDPORTE PORTB5
#define LEDPORTF PORTB6
#define LEDPORTG PORTB7
//#define LEDPORTDOT XXXX // decimal not acessable on module
#define LEDPORTDOT PORTB4 // colon, unused

#define LEDDIGDDR DDRC
#define LEDDIGPORT PORTC
#define LEDDIGPORT1 PORTC2
#define LEDDIGPORT2 PORTC4
#define LEDDIGPORT3 PORTC5
#define LEDDIGPORT4 PORTC6

//Button definitions
#define BUTTONDDR DDRD
#define BUTTONPORT PORTD
#define BUTTONPORT_Z PORTD5
#define BUTTONPORT_UP PORTD6
#define BUTTONPORT_DN PORTD7
#define BUTTONPIN PIND
#define BUTTONPIN_Z PIND5
#define BUTTONPIN_UP PIND6
#define BUTTONPIN_DN PIND7

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

signed char Position = 0;
unsigned char ScanPos = 0; // Digit to illuminate in the scan
unsigned char DigitSign = 0; // Takes place of DigitNum1
unsigned char DigitNum2 = 0; // Second digit from left
unsigned char DigitNum3 = 0; // Third digit from left
unsigned char DigitNum4 = 0; // Fourth digit from left

//------------------------------------------------------------------------------
// Function prototypes
//------------------------------------------------------------------------------

void InitIO(void);
void InitTimer(void);
void SetLEDDigit(unsigned char Num, unsigned char DigPortNum);
void ClearLEDSegments(void);
void SetLEDSign(unsigned char Sign, unsigned char DigPortNum);
void SetDigit1(void);
void SetDigit2(void);
void SetDigit3(void);
void SetDigit4(void);

//Temp
void FutureButtonISR(void);

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

void InitIO(void)
{

  LEDDDR |= (_BV(LEDPORTA) | // Set as outputs
             _BV(LEDPORTB) |
             _BV(LEDPORTC) |
             _BV(LEDPORTD) |
             _BV(LEDPORTE) |
             _BV(LEDPORTF) |
             _BV(LEDPORTG) |
             _BV(LEDPORTDOT));

  LEDDIGDDR |= (_BV(LEDDIGPORT1) | // Set as outputs
                _BV(LEDDIGPORT2) |
                _BV(LEDDIGPORT3) |
                _BV(LEDDIGPORT4));


  BUTTONDDR &= ~(_BV(BUTTONPORT_Z) | // Set as inputs
                 _BV(BUTTONPORT_UP) |
                 _BV(BUTTONPORT_DN));
  BUTTONPORT |= (_BV(BUTTONPORT_Z) | // Enable internal pullup resistors
                 _BV(BUTTONPORT_UP) |
                 _BV(BUTTONPORT_DN));

  LEDDIGPORT &= ~(_BV(LEDDIGPORT1) | // Turn digits off
                  _BV(LEDDIGPORT2) |
                  _BV(LEDDIGPORT3) |
                  _BV(LEDDIGPORT4));

  LEDPORT &= ~(_BV(LEDPORTDOT)); // Turn dots off
  LEDPORT |= (_BV(LEDPORTA) | // Turn segments off (active low)
              _BV(LEDPORTB) |
              _BV(LEDPORTC) |
              _BV(LEDPORTD) |
              _BV(LEDPORTE) |
              _BV(LEDPORTF) |
              _BV(LEDPORTG));

}

//------------------------------------------------------------------------------

void InitTimer(void)
{

  cli(); // Disable global interrupts

  TCCR0A |= _BV(WGM01); // Put Timer/Counter0 in CTC mode
  TCCR0B |= (_BV(CS01) | // Set the prescaler to 64
             _BV(CS00));
  OCR0A = 249; // Set the A compare value to interrupt timer at 
               // (16000 cycles = 1ms prescaled at 64)
  TIMSK0 |= _BV(OCIE0A); // Enable timer compare A interrupt

  sei(); // Enable global interrupts

}

//------------------------------------------------------------------------------
// Interrupt driven display driver, each interrupt increments the ScanPos and
// illuminates the next digit display.
ISR(TIMER0_COMPA_vect)
{

  switch(++ScanPos)
  {
  case 1:
    SetDigit1();
    break;
  case 2:
    SetDigit2();
    break;
  case 3:
    SetDigit3();
    break;
  case 4:
    SetDigit4();
    break;
  default:
    ScanPos = 0; // Reset the positiion
    break;
  }

  if(ScanPos >= 4)
  {
    ScanPos = 0; // Reset the position
  }
}

//------------------------------------------------------------------------------
// Turns off the 7 segment LED digit
// LED display is active low
void ClearLEDSegments(void)
{

  LEDPORT |= (_BV(LEDPORTA) | // Turn of segments
              _BV(LEDPORTB) |
              _BV(LEDPORTC) |
              _BV(LEDPORTD) |
              _BV(LEDPORTE) |
              _BV(LEDPORTF) |
              _BV(LEDPORTG));

}

//------------------------------------------------------------------------------
// Sets the 7 segment LED to the number passed in the argument
// LED display is active low
void SetLEDDigit(unsigned char Num, unsigned char DigPortNum)
{

	switch(Num)
	{
        case 0:
          
          ClearLEDSegments();
          LEDPORT &= ~(_BV(LEDPORTA) | // Turn on segments
                       _BV(LEDPORTB) |
                       _BV(LEDPORTC) |
                       _BV(LEDPORTD) |
                       _BV(LEDPORTE) |
                       _BV(LEDPORTF));
          break;
          
        case 1:

          ClearLEDSegments();
          LEDPORT &= ~(_BV(LEDPORTB) | // Turn on segments
                       _BV(LEDPORTC));
          break;
                      
        case 2:
          
          ClearLEDSegments();
          LEDPORT &= ~(_BV(LEDPORTA) | // Turn on segments
                       _BV(LEDPORTB) |
                       _BV(LEDPORTD) |
                       _BV(LEDPORTE) |
                       _BV(LEDPORTG));
          break;
          
        case 3:
          
          ClearLEDSegments();
          LEDPORT &= ~(_BV(LEDPORTA) | // Turn on segments
                       _BV(LEDPORTB) |
                       _BV(LEDPORTC) |
                       _BV(LEDPORTD) |
                       _BV(LEDPORTG));
          break;
          
        case 4:
          
          ClearLEDSegments();
          LEDPORT &= ~(_BV(LEDPORTB) | // Turn on segments
                       _BV(LEDPORTC) |
                       _BV(LEDPORTF) |
                       _BV(LEDPORTG));
          break;
          
        case 5:
          
          ClearLEDSegments();
          LEDPORT &= ~(_BV(LEDPORTA) | // Turn on segments
                       _BV(LEDPORTC) |
                       _BV(LEDPORTD) |
                       _BV(LEDPORTF) |
                       _BV(LEDPORTG));
          break;
          
        case 6:
          
          ClearLEDSegments();
          LEDPORT &= ~(_BV(LEDPORTA) | // Turn on segments
                       _BV(LEDPORTC) |
                       _BV(LEDPORTD) |
                       _BV(LEDPORTE) |
                       _BV(LEDPORTF) |
                       _BV(LEDPORTG));
          break;
          
        case 7:
          
          ClearLEDSegments();
          LEDPORT &= ~(_BV(LEDPORTA) | // Turn on segments
                       _BV(LEDPORTB) |
                       _BV(LEDPORTC));
          break;
          
        case 8:
          
          // Clearing not required since we're setting all on
          LEDPORT &= ~(_BV(LEDPORTA) | // Turn on segments
                       _BV(LEDPORTB) |
                       _BV(LEDPORTC) |
                       _BV(LEDPORTD) |
                       _BV(LEDPORTE) |
                       _BV(LEDPORTF) |
                       _BV(LEDPORTG));
          break;
          
        case 9:
          
          ClearLEDSegments();
          LEDPORT &= ~(_BV(LEDPORTA) | // Turn on segments
                       _BV(LEDPORTB) |
                       _BV(LEDPORTC) |
                       _BV(LEDPORTD) |
                       _BV(LEDPORTF) |
                       _BV(LEDPORTG));
          break;
          
        default:

          break;
          
          
        }

        // Segments are ready, now select the digit
        LEDDIGPORT &= ~(_BV(LEDDIGPORT1) | // Turn off all digits
                        _BV(LEDDIGPORT2) |
                        _BV(LEDDIGPORT3) |
                        _BV(LEDDIGPORT4));
        LEDDIGPORT |= _BV(DigPortNum);

}

//------------------------------------------------------------------------------
// If Sign is non-zero, sets the negative sign, if zero, clears the negative
// sign on the digit 
void SetLEDSign(unsigned char Sign, unsigned char DigPortNum)
{

  ClearLEDSegments(); // Turn off all segments
  if(Sign)
  {
    LEDPORT &= ~_BV(LEDPORTG); // Negative (non-zero), turn on segment
  }

  // Segments are ready, now select the digit
  LEDDIGPORT &= ~(_BV(LEDDIGPORT1) | // Turn off all digits
                  _BV(LEDDIGPORT2) |
                  _BV(LEDDIGPORT3) |
                  _BV(LEDDIGPORT4));
  LEDDIGPORT |= _BV(DigPortNum); // Turn on the digit

}

//------------------------------------------------------------------------------
void FutureButtonISR()
{

  // Debounce

  // If UP button, increment Position unless at 100

  // If DN button, decrement Position unless at -100

  // If Z button, set Position to 0

}

//------------------------------------------------------------------------------
// Sets the first display digit, which is the sign
void SetDigit1(void)
{

  if(Position < 0)
  {
    SetLEDSign(1, LEDDIGPORT1);
  }
  else
  {
    SetLEDSign(0, LEDDIGPORT1);
  }

}

//------------------------------------------------------------------------------
// Sets the second display digit, which is the leading number (10^2)
//
// *** Needs ABS
//
void SetDigit2(void)
{
  if(Position / 100)
  {
    SetLEDDigit(abs(Position / 100), LEDDIGPORT2);
  }

}

//------------------------------------------------------------------------------
// Sets the third display digit, which is the middle number (10^1)
//
// *** Needs ABS
//
void SetDigit3(void)
{
  if(Position / 10)
  {
    SetLEDDigit(abs((Position - 
                     ((Position / 100) * 100)) / 10),
                LEDDIGPORT3);
  }

}

//------------------------------------------------------------------------------
// Sets the fourth display digit, which is the last number (10^0)
//
// *** Currently broken
//
void SetDigit4(void)
{
  SetLEDDigit(abs(Position % 10), LEDDIGPORT4);
}

//==============================================================================
int main (void)
{

  //Do setup here
  InitIO();
  InitTimer();

  // For testing only
  Position = -20;


  //Main program loop
  while (1)
  {

    _delay_ms(1000);
    Position++;

  }

}

//==============================================================================
/* End of code, junk below



    // Loop until either UP or DN is low (key press)
    while ((BUTTONPIN & _BV(BUTTONPIN_DN)) &&
           (BUTTONPIN & _BV(BUTTONPIN_UP)))
    {

      switch (BUTTONPIN & (_BV(BUTTONPIN_UP) |
                           _BV(BUTTONPIN_DN)))
      {
      case _BV(BUTTONPIN_UP):
        LEDPORT &= ~_BV(LEDPORT0); // Turn off LED0
        break;
      case _BV(BUTTONPIN_DN):
        LEDPORT |= _BV(LEDPORT0); // Turn on LED0
        break;
      default:
        break;
      }

    }


*/

#include "shift.h"

void shift_init(){
	//Enable outputs for shift register (clock LED driver)
	SHIFT_DDR |= _BV(SHIFT_DATA_PIN) | _BV(SHIFT_CLOCK_PIN) | _BV(SHIFT_LATCH_PIN);
}

void shift_out(uint8_t data){
	//We use transistors to bump up voltage; as a side effect they reverse the logic 
	// signals too, so we clock lowering edge which in effect is a rising edge.

	data = ~data; //Bitwise invert the data to account for the transistors.
	for (int i = 0; i < 8; i++){
		//Clear the data pin first...
		SHIFT_PORT &= ~_BV(SHIFT_DATA_PIN);
		//... then set the bit (if appropriate).  We could probably
		// do this in one step, but this is more clear, plus speed is 
		// (probably) not critical here.
		SHIFT_PORT |= (((data >> (7 - i)) & 0x1) << SHIFT_DATA_PIN);

		//Pulse clock to shift in.  Remember the inverted logic levels...
		SHIFT_PORT |= _BV(SHIFT_CLOCK_PIN); //Bring the shift register clock input low
		_delay_us(1);
		SHIFT_PORT &= ~_BV(SHIFT_CLOCK_PIN); //Pull the shift register clock input high
	}
}

void shift_latch(){
	//Pulse latch to transfer contents to output.  We use transistors to bump up voltage;
	// as a side effect they reverse the logic signals too, so we clock lowering edge
	// which in effect is a rising edge.	
	SHIFT_PORT &= ~_BV(SHIFT_LATCH_PIN);	//Set pin low, i.e. shift latch input high
	_delay_us(1);							//We need a slight delay here, or else the shift register only detects the change occasionally.
	SHIFT_PORT |= _BV(SHIFT_LATCH_PIN);		//Reset shift latch to low; i.e no change on outputs
}

void shift_format_data(uint8_t hours, uint8_t minutes, uint8_t flags, uint32_t *data1, uint32_t *data2){	
	//We have four digits: H2, H1, M2, M1.  (H2 is MSB of Hour).  Each of these digits has
	// 7 segments: A .. F.  Each segment is lit by a (pin,pin) combination, with the pin
	// numbers mapping to the LED panel pins, with the first
	// pin being the cathode (pin 1 or 2), and the second pin being the anode pin.  We 
	// map these combinations out below, above the defines for each digit.

	//Here we map between LED pins to shift register outputs, counting down from bit 14: 
	// LED Pin	Shift Data		Shift Pin
	// 4 (Auto)	23				3.11
	// 2 (C1)	17				3.11
	// 1 (C2)	16				3.12
	// 30/14(:)	15				3.11
	// 5 (PM)	14				2.11
	// 6		13				2.12
	// 7		12				2.13
	// 8						2.14
	// 9		8				2.4
	// 10		9				2.5
	// 11		10				2.6
	// 12		11				2.7
	// 13		7				1.11
	// 14		7				1.14
	// 15		6				1.13
	// 16		5				1.12
	// 17		4				1.11
	// 18		0				1.4
	// 19		1				1.5
	// 20		2				1.6
	// 21		3				1.7
	
	//Defines mappings between Led Pin X and the data packet index
	#define LP1		23
	#define LP2		22
	#define LP3		//Connected to LP2
	#define LP4		18
	#define LP5		19
	#define LP6		12
	#define LP7		13
	#define LP8		14
	#define LP9		15
	#define LP10	8
	#define LP11	9
	#define LP12	10
	#define LP13	11
	#define LP14	4
	#define LP15	5
	#define LP16	6
	#define LP17	7
	#define LP18	0
	#define LP19	1
	#define LP20	2
	#define LP21	3		
	
	#define CATHODE_1	_BV32(LP1)
	#define CATHODE_2	_BV32(LP2)
	
	#define COLON_1	_BV32(LP14)	//If we use normal _BV() it assumes unsigned, and fills in all 1's
	#define COLON_2	0
	#define AUTO_1	0
	#define AUTO_2	_BV32(LP4)
	#define PM_1	_BV32(LP5)
	#define PM_2	0

	//    HourMinNumber_Segment_Cathode
	// H2: A=(1,7), B=(2,6), C=(2,9), D=(1,8), E=(2,8), G=(2,7) (F is not implemented, so 0, 4, 5, 6, 8, 9 are not possible)
	#define H2_A_1 	_BV32(LP7)
	#define H2_A_2 	0
	#define H2_B_1 	0
	#define H2_B_2	_BV32(LP6)
	#define H2_C_1	0
	#define H2_C_2	_BV32(LP9)
	#define H2_D_1	_BV32(LP8)
	#define H2_D_2	0
	#define H2_E_1	0
	#define H2_E_2	_BV32(LP8)
	#define H2_F_1	0
	#define H2_F_2	0
	#define H2_G_1	0
	#define H2_G_2	_BV32(LP7)
	// 8		8				2.4
	// 9		9				2.5
	// 10		10				2.6
	// 12		11				2.7
		
	// H1: A=(2,13), B=(2,10), C=(2,12), D=(1,12), E=(1,9), F=(1,13), G=(1,10)
	#define H1_A_1	0
	#define H1_A_2	_BV32(LP13)
	#define H1_B_1	0
	#define H1_B_2	_BV32(LP10)
	#define H1_C_1	0
	#define H1_C_2	_BV32(LP12)
	#define H1_D_1	_BV32(LP12)
	#define H1_D_2	0
	#define H1_E_1	_BV32(LP9)
	#define H1_E_2	0
	#define H1_F_1	_BV32(LP13)
	#define H1_F_2	0
	#define H1_G_1	_BV32(LP10)
	#define H1_G_2	0
	
	// M2: A=(1,15), B=(1,16), C=(1,17), F=(2,15), G=(2,16), D=(2,17), E=(2,18)
	#define M2_A_1	_BV(LP15)
	#define M2_A_2	0
	#define M2_B_1	_BV(LP16)
	#define M2_B_2	0
	#define M2_C_1	_BV(LP17)
	#define M2_C_2	0
	#define M2_D_1	0
	#define M2_D_2	_BV(LP17)
	#define M2_E_1	0
	#define M2_E_2	_BV(LP18)
	#define M2_F_1	0
	#define M2_F_2	_BV(LP15)
	#define M2_G_1	0
	#define M2_G_2	_BV(LP16)

	// M1: E=(1,18), G=(1,19), D=(1,20), F=(1,21), B=(2,19), C=(2,20), A=(2,21)
	#define M1_A_1	0
	#define M1_A_2	_BV(LP21)
	#define M1_B_1	0
	#define M1_B_2	_BV(LP19)
	#define M1_C_1	0
	#define M1_C_2	_BV(LP20)
	#define M1_D_1	_BV(LP20)
	#define M1_D_2	0
	#define M1_E_1	_BV(LP18)
	#define M1_E_2	0
	#define M1_F_1	_BV(LP21)
	#define M1_F_2	0
	#define M1_G_1	_BV(LP19)
	#define M1_G_2	0

	
	//Clears existing data, and set the cathode to use
	*data1 = CATHODE_1;
	*data2 = CATHODE_2;
	
	if (flags & SHIFT_FLAG_COLON){
		*data1 |= COLON_1;
		*data2 |= COLON_2;
	}
	if (flags & SHIFT_FLAG_PM){
		*data1 |= PM_1;
		*data2 |= PM_2;
	}
	if (flags & SHIFT_FLAG_AUTO){
		*data1 |= AUTO_1;
		*data2 |= AUTO_2;
	}

	//Set MSB Hour; only 1, 2, and 3 are supported.
	switch (hours >> 4) {
		case 0x01:
			*data1 |= H2_B_1 | H2_C_1;
			*data2 |= H2_B_2 | H2_C_2;
			break;
			
		case 0x02:
			*data1 |= H2_A_1 | H2_B_1 | H2_D_1 | H2_E_1 | H2_G_1;
			*data2 |= H2_A_2 | H2_B_2 | H2_D_2 | H2_E_2 | H2_G_2;
			break;

		case 0x03:
			*data1 |= H2_A_1 | H2_B_1 | H2_C_1 | H2_D_1 | H2_G_1;
			*data2 |= H2_A_2 | H2_B_2 | H2_C_2 | H2_D_2 | H2_G_2;
			break;
	}
	
	//Set LSB hour
	switch (hours & 0xF) {
		case 0x00:
			*data1 |= H1_A_1 | H1_B_1 | H1_C_1 | H1_D_1 | H1_E_1 | H1_F_1;
			*data2 |= H1_A_2 | H1_B_2 | H1_C_2 | H1_D_2 | H1_E_2 | H1_F_2;
			break;

		case 0x01:
			*data1 |= H1_B_1 | H1_C_1;
			*data2 |= H1_B_2 | H1_C_2;
			break;
			
		case 0x02:
			*data1 |= H1_A_1 | H1_B_1 | H1_D_1 | H1_E_1 | H1_G_1;
			*data2 |= H1_A_2 | H1_B_2 | H1_D_2 | H1_E_2 | H1_G_2;
			break;

		case 0x03:
			*data1 |= H1_A_1 | H1_B_1 | H1_C_1 | H1_D_1 | H1_G_1;
			*data2 |= H1_A_2 | H1_B_2 | H1_C_2 | H1_D_2 | H1_G_2;
			break;

		case 0x04:
			*data1 |= H1_B_1 | H1_C_1 | H1_F_1 | H1_G_1;
			*data2 |= H1_B_2 | H1_C_2 | H1_F_2 | H1_G_2;
			break;

		case 0x05:
			*data1 |= H1_A_1 | H1_C_1 | H1_D_1 | H1_F_1 | H1_G_1;
			*data2 |= H1_A_2 | H1_C_2 | H1_D_2 | H1_F_2 | H1_G_2;
			break;

		case 0x06:
			*data1 |= H1_A_1 | H1_C_1 | H1_D_1 | H1_E_1 | H1_F_1 | H1_G_1;
			*data2 |= H1_A_2 | H1_C_2 | H1_D_2 | H1_E_2 | H1_F_2 | H1_G_2;
			break;

		case 0x07:
			*data1 |= H1_A_1 | H1_B_1 | H1_C_1;
			*data2 |= H1_A_2 | H1_B_2 | H1_C_2;
			break;

		case 0x08:
			*data1 |= H1_A_1 | H1_B_1 | H1_C_1 | H1_D_1 | H1_E_1 | H1_F_1 | H1_G_1;
			*data2 |= H1_A_2 | H1_B_2 | H1_C_2 | H1_D_2 | H1_E_2 | H1_F_2 | H1_G_2;
			break;

		case 0x09:
			*data1 |= H1_A_1 | H1_B_1 | H1_C_1 | H1_D_1 | H1_F_1 | H1_G_1;
			*data2 |= H1_A_2 | H1_B_2 | H1_C_2 | H1_D_2 | H1_F_2 | H1_G_2;
			break;

		case 0x0A:
			*data1 |= H1_A_1 | H1_B_1 | H1_C_1 | H1_E_1 | H1_F_1 | H1_G_1;
			*data2 |= H1_A_2 | H1_B_2 | H1_C_2 | H1_E_2 | H1_F_2 | H1_G_2;
			break;

		case 0x0B:
			*data1 |= H1_C_1 | H1_D_1 | H1_E_1 | H1_F_1 | H1_G_1;
			*data2 |= H1_C_2 | H1_D_2 | H1_E_2 | H1_F_2 | H1_G_2;
			break;

		case 0x0C:
			*data1 |= H1_A_1 | H1_D_1 | H1_E_1 | H1_F_1;
			*data2 |= H1_A_2 | H1_D_2 | H1_E_2 | H1_F_2;
			break;

		case 0x0D:
			*data1 |= H1_B_1 | H1_C_1 | H1_D_1 | H1_E_1 | H1_G_1;
			*data2 |= H1_B_2 | H1_C_2 | H1_D_2 | H1_E_2 | H1_G_2;
			break;

		case 0x0E:
			*data1 |= H1_A_1 | H1_D_1 | H1_E_1 | H1_F_1 | H1_G_1;
			*data2 |= H1_A_2 | H1_D_2 | H1_E_2 | H1_F_2 | H1_G_2;
			break;

		case 0x0F:
			*data1 |= H1_A_1 | H1_E_1 | H1_F_1 | H1_G_1;
			*data2 |= H1_A_2 | H1_E_2 | H1_F_2 | H1_G_2;
			break;

	}
			
	//Set MSB minute
	switch (minutes >> 4) {
		case 0x00:
			*data1 |= M2_A_1 | M2_B_1 | M2_C_1 | M2_D_1 | M2_E_1 | M2_F_1;
			*data2 |= M2_A_2 | M2_B_2 | M2_C_2 | M2_D_2 | M2_E_2 | M2_F_2;
			break;

		case 0x01:
			*data1 |= M2_B_1 | M2_C_1;
			*data2 |= M2_B_2 | M2_C_2;
			break;
			
		case 0x02:
			*data1 |= M2_A_1 | M2_B_1 | M2_D_1 | M2_E_1 | M2_G_1;
			*data2 |= M2_A_2 | M2_B_2 | M2_D_2 | M2_E_2 | M2_G_2;
			break;

		case 0x03:
			*data1 |= M2_A_1 | M2_B_1 | M2_C_1 | M2_D_1 | M2_G_1;
			*data2 |= M2_A_2 | M2_B_2 | M2_C_2 | M2_D_2 | M2_G_2;
			break;

		case 0x04:
			*data1 |= M2_B_1 | M2_C_1 | M2_F_1 | M2_G_1;
			*data2 |= M2_B_2 | M2_C_2 | M2_F_2 | M2_G_2;
			break;

		case 0x05:
			*data1 |= M2_A_1 | M2_C_1 | M2_D_1 | M2_F_1 | M2_G_1;
			*data2 |= M2_A_2 | M2_C_2 | M2_D_2 | M2_F_2 | M2_G_2;
			break;

		case 0x06:
			*data1 |= M2_A_1 | M2_C_1 | M2_D_1 | M2_E_1 | M2_F_1 | M2_G_1;
			*data2 |= M2_A_2 | M2_C_2 | M2_D_2 | M2_E_2 | M2_F_2 | M2_G_2;
			break;

		case 0x07:
			*data1 |= M2_A_1 | M2_B_1 | M2_C_1;
			*data2 |= M2_A_2 | M2_B_2 | M2_C_2;
			break;

		case 0x08:
			*data1 |= M2_A_1 | M2_B_1 | M2_C_1 | M2_D_1 | M2_E_1 | M2_F_1 | M2_G_1;
			*data2 |= M2_A_2 | M2_B_2 | M2_C_2 | M2_D_2 | M2_E_2 | M2_F_2 | M2_G_2;
			break;

		case 0x09:
			*data1 |= M2_A_1 | M2_B_1 | M2_C_1 | M2_D_1 | M2_F_1 | M2_G_1;
			*data2 |= M2_A_2 | M2_B_2 | M2_C_2 | M2_D_2 | M2_F_2 | M2_G_2;
			break;

		case 0x0A:
			*data1 |= M2_A_1 | M2_B_1 | M2_C_1 | M2_E_1 | M2_F_1 | M2_G_1;
			*data2 |= M2_A_2 | M2_B_2 | M2_C_2 | M2_E_2 | M2_F_2 | M2_G_2;
			break;

		case 0x0B:
			*data1 |= M2_C_1 | M2_D_1 | M2_E_1 | M2_F_1 | M2_G_1;
			*data2 |= M2_C_2 | M2_D_2 | M2_E_2 | M2_F_2 | M2_G_2;
			break;

		case 0x0C:
			*data1 |= M2_A_1 | M2_D_1 | M2_E_1 | M2_F_1;
			*data2 |= M2_A_2 | M2_D_2 | M2_E_2 | M2_F_2;
			break;

		case 0x0D:
			*data1 |= M2_B_1 | M2_C_1 | M2_D_1 | M2_E_1 | M2_G_1;
			*data2 |= M2_B_2 | M2_C_2 | M2_D_2 | M2_E_2 | M2_G_2;
			break;

		case 0x0E:
			*data1 |= M2_A_1 | M2_D_1 | M2_E_1 | M2_F_1 | M2_G_1;
			*data2 |= M2_A_2 | M2_D_2 | M2_E_2 | M2_F_2 | M2_G_2;
			break;

		case 0x0F:
			*data1 |= M2_A_1 | M2_E_1 | M2_F_1 | M2_G_1;
			*data2 |= M2_A_2 | M2_E_2 | M2_F_2 | M2_G_2;
			break;
	}
			
	//Set LSB minute
	switch (minutes & 0x0F) {
		case 0x00:
			*data1 |= M1_A_1 | M1_B_1 | M1_C_1 | M1_D_1 | M1_E_1 | M1_F_1;
			*data2 |= M1_A_2 | M1_B_2 | M1_C_2 | M1_D_2 | M1_E_2 | M1_F_2;
			break;

		case 0x01:
			*data1 |= M1_B_1 | M1_C_1;
			*data2 |= M1_B_2 | M1_C_2;
			break;
			
		case 0x02:
			*data1 |= M1_A_1 | M1_B_1 | M1_D_1 | M1_E_1 | M1_G_1;
			*data2 |= M1_A_2 | M1_B_2 | M1_D_2 | M1_E_2 | M1_G_2;
			break;

		case 0x03:
			*data1 |= M1_A_1 | M1_B_1 | M1_C_1 | M1_D_1 | M1_G_1;
			*data2 |= M1_A_2 | M1_B_2 | M1_C_2 | M1_D_2 | M1_G_2;
			break;

		case 0x04:
			*data1 |= M1_B_1 | M1_C_1 | M1_F_1 | M1_G_1;
			*data2 |= M1_B_2 | M1_C_2 | M1_F_2 | M1_G_2;
			break;

		case 0x05:
			*data1 |= M1_A_1 | M1_C_1 | M1_D_1 | M1_F_1 | M1_G_1;
			*data2 |= M1_A_2 | M1_C_2 | M1_D_2 | M1_F_2 | M1_G_2;
			break;

		case 0x06:
			*data1 |= M1_A_1 | M1_C_1 | M1_D_1 | M1_E_1 | M1_F_1 | M1_G_1;
			*data2 |= M1_A_2 | M1_C_2 | M1_D_2 | M1_E_2 | M1_F_2 | M1_G_2;
			break;

		case 0x07:
			*data1 |= M1_A_1 | M1_B_1 | M1_C_1;
			*data2 |= M1_A_2 | M1_B_2 | M1_C_2;
			break;

		case 0x08:
			*data1 |= M1_A_1 | M1_B_1 | M1_C_1 | M1_D_1 | M1_E_1 | M1_F_1 | M1_G_1;
			*data2 |= M1_A_2 | M1_B_2 | M1_C_2 | M1_D_2 | M1_E_2 | M1_F_2 | M1_G_2;
			break;

		case 0x09:
			*data1 |= M1_A_1 | M1_B_1 | M1_C_1 | M1_D_1 | M1_F_1 | M1_G_1;
			*data2 |= M1_A_2 | M1_B_2 | M1_C_2 | M1_D_2 | M1_F_2 | M1_G_2;
			break;

		case 0x0A:
			*data1 |= M1_A_1 | M1_B_1 | M1_C_1 | M1_E_1 | M1_F_1 | M1_G_1;
			*data2 |= M1_A_2 | M1_B_2 | M1_C_2 | M1_E_2 | M1_F_2 | M1_G_2;
			break;

		case 0x0B:
			*data1 |= M1_C_1 | M1_D_1 | M1_E_1 | M1_F_1 | M1_G_1;
			*data2 |= M1_C_2 | M1_D_2 | M1_E_2 | M1_F_2 | M1_G_2;
			break;

		case 0x0C:
			*data1 |= M1_A_1 | M1_D_1 | M1_E_1 | M1_F_1;
			*data2 |= M1_A_2 | M1_D_2 | M1_E_2 | M1_F_2;
			break;

		case 0x0D:
			*data1 |= M1_B_1 | M1_C_1 | M1_D_1 | M1_E_1 | M1_G_1;
			*data2 |= M1_B_2 | M1_C_2 | M1_D_2 | M1_E_2 | M1_G_2;
			break;

		case 0x0E:
			*data1 |= M1_A_1 | M1_D_1 | M1_E_1 | M1_F_1 | M1_G_1;
			*data2 |= M1_A_2 | M1_D_2 | M1_E_2 | M1_F_2 | M1_G_2;
			break;

		case 0x0F:
			*data1 |= M1_A_1 | M1_E_1 | M1_F_1 | M1_G_1;
			*data2 |= M1_A_2 | M1_E_2 | M1_F_2 | M1_G_2;
			break;
		
	}
}

/*****************************************************************************
* I2C master library, originally based off of Atmel Corp's AVR315 application
* note.  Simplified and customized by Wyatt Olson to work better as a library.
*
* Usage:
* 	Master:
*		1) Call isc_master_init()
*		2) Create a uint8_t message[] of X length, with byte 0 being the 
*		   slave address (bits 7::1) with bit 0 being R/W (I2C_READ / I2C_WRITE)
*		   and the other bits being data you wish to send (if write mode) or data
*		   you wish to read into (if read mode).
*		3) Call i2c_start_transceiver_with_data passing in the message and message length.
*		4) (If in Read mode) Call i2c_get_data_from_transceiver to copy the data to another buffer
****************************************************************************/

#include "i2c_master.h"

//RX / TX buffer.  Size should be greater than the largest message which is to be sent.
static uint8_t _i2c_buf[32];

// Number of bytes to be transmitted.
static uint8_t _i2c_msg_size;

// State byte. Default set to TWI_NO_STATE.
static uint8_t _i2c_state = TWI_NO_STATE;		

//Status
static uint8_t _i2c_status;

/*
 * Set up the I2C hardware, at the specified i2c clock frequency (in MHz).
 */
void i2c_master_init(int16_t i2c_clock_frequency){
	// Calculate bit rate register value given FCPU and desired frequency.  Assume prescaler is 0.
	TWBR = ((F_CPU / ((i2c_clock_frequency) * 1000)) - 16) / 2;
	//TWSR = 0x0;                                   // Prescaler
	TWDR = 0xFF;                                    // Default content = SDA released.
	TWCR = (1<<TWEN)|                                 // Enable TWI-interface and release TWI pins.
		(0<<TWIE)|(0<<TWINT)|                      // Disable Interupt.
		(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // No Signal requests.
		(0<<TWWC);                                 //
		
	sei();
}    
    
/*
 * Checks if the ISR is busy transmitting.  Used internally.
 */
uint8_t _i2c_transceiver_busy(){
	return ( TWCR & (1<<TWIE) );                  // IF TWI Interrupt is enabled then the Transceiver is busy
}

/*
 * Returns the state of the last operation.
 */
uint8_t i2c_get_state_info(){
	while (_i2c_transceiver_busy());             // Wait until TWI has completed the transmission.
	return ( _i2c_state );                         // Return error state.
}

/*
 * Send a prepared message.  The first byte is the slave address (bits 7:1) and the R/W bit (bit 0).
 * You can use constants I2C_WRITE and I2C_READ bitwise-or'd with the shifted address; for instance,
 * message[0] = ADDRESS << 1 | I2C_READ.
 * This function will block until the ISR has completed the previous operation (if any), then 
 * start the next operation and return immediately.
 */
void i2c_start_transceiver_with_data(uint8_t *message, uint8_t message_size){
	uint8_t temp;

	while (_i2c_transceiver_busy());					// Wait until TWI is ready for next transmission.

	_i2c_msg_size = message_size;						// Number of data to transmit.
	_i2c_buf[0]  = message[0];							// Store slave address with R/W setting.
	if (!(message[0] & (I2C_WRITE << TWI_READ_BIT))){	// If it is a write operation, then also copy data.
		for (temp = 1; temp < message_size; temp++){
			_i2c_buf[temp] = message[temp];
		}
	}
	_i2c_status = 0;      
	_i2c_state = TWI_NO_STATE ;
	TWCR = (1<<TWEN)|                             // TWI Interface enabled.
		(1<<TWIE)|(1<<TWINT)|                  // Enable TWI Interupt and clear the flag.
		(0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|       // Initiate a START condition.
		(0<<TWWC);                             //
}

/*
 * Resend the last message (transmit a SR condition).  We will reuse the existing message buffer
 * from the last call to i2c_start_transceiver_with_data, will block until the last operation
 * is completed, and will then start the next operation and return immediately.
 */
void i2c_start_transceiver(){
	while (_i2c_transceiver_busy());			// Wait until TWI is ready for next transmission.
	_i2c_status = 0;      
	_i2c_state         = TWI_NO_STATE ;
	TWCR = (1<<TWEN)|							// TWI Interface enabled.
		(1<<TWIE)|(1<<TWINT)|					// Enable TWI Interupt and clear the flag.
		(0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|		// Initiate a START condition.
		(0<<TWWC);								//
}

/*
 * Reads the data from the last isc_start_transceiver*() function.  This function will
 * block until the last transfer operation is complete, and then copy the result of the
 * last operation into *message (up to message_size).
 */
/****************************************************************************
Call this function to read out the requested data from the TWI transceiver buffer. I.e. first call
TWI_Start_Transceiver to send a request for data to the slave. Then Run this function to collect the
data when they have arrived. Include a pointer to where to place the data and the number of bytes
requested (including the address field) in the function call. The function will hold execution (loop)
until the TWI_ISR has completed with the previous operation, before reading out the data and returning.
If there was an error in the previous transmission the function will return the TWI error code.
****************************************************************************/
uint8_t i2c_get_data_from_transceiver(uint8_t *message, uint8_t message_size){
	uint8_t i;

	while (_i2c_transceiver_busy());             // Wait until TWI is ready for next transmission.

	if(_i2c_status){							// Last transmission competed successfully.              
	    for (i = 0; i < message_size; i++){				// Copy data from Transceiver buffer.
			message[i] = _i2c_buf[i];
	    }
	  }
	return( _i2c_status );
}

/*
 * This is the ISR called when TWI interrupt is triggered.
 */
ISR(TWI_vect){
	static uint8_t _i2c_buf_ptr;
  
	switch (TWSR){
		case TWI_START:				// START has been transmitted  
		case TWI_REP_START:			// Repeated START has been transmitted
			_i2c_buf_ptr = 0;									// Set buffer pointer to the TWI Address location
		case TWI_MTX_ADR_ACK:		// SLA+W has been tramsmitted and ACK received
		case TWI_MTX_DATA_ACK:		// Data byte has been tramsmitted and ACK received
			if (_i2c_buf_ptr < _i2c_msg_size){
				TWDR = _i2c_buf[_i2c_buf_ptr++];
				TWCR = (1<<TWEN)|								// TWI Interface enabled
					(1<<TWIE)|(1<<TWINT)|						// Enable TWI Interupt and clear the flag to send byte
					(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|			//
					(0<<TWWC);									//  
			}
			else{                    							// Send STOP after last byte
				_i2c_status = 1;								// Set status bits to completed successfully. 
				TWCR = (1<<TWEN)|								// TWI Interface enabled
					(0<<TWIE)|(1<<TWINT)|						// Disable TWI Interrupt and clear the flag
					(0<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|			// Initiate a STOP condition.
					(0<<TWWC);									//
			}
			break;
		case TWI_MRX_DATA_ACK:		// Data byte has been received and ACK tramsmitted
			_i2c_buf[_i2c_buf_ptr++] = TWDR;
		case TWI_MRX_ADR_ACK:		// SLA+R has been tramsmitted and ACK received
			if (_i2c_buf_ptr < (_i2c_msg_size - 1)){			// Detect the last byte to NACK it.
				TWCR = (1<<TWEN)|								// TWI Interface enabled
					(1<<TWIE)|(1<<TWINT)|						// Enable TWI Interupt and clear the flag to read next byte
					(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|			// Send ACK after reception
					(0<<TWWC);									//  
			}
			else {												// Send NACK after next reception
				TWCR = (1<<TWEN)|								// TWI Interface enabled
					(1<<TWIE)|(1<<TWINT)|						// Enable TWI Interupt and clear the flag to read next byte
					(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|			// Send NACK after reception
					(0<<TWWC);									// 
			}    
			break; 
		case TWI_MRX_DATA_NACK:		// Data byte has been received and NACK tramsmitted
			_i2c_buf[_i2c_buf_ptr] = TWDR;
			_i2c_status = 1;									// Set status bits to completed successfully. 
			TWCR = (1<<TWEN)|									// TWI Interface enabled
				(0<<TWIE)|(1<<TWINT)|							// Disable TWI Interrupt and clear the flag
				(0<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|				// Initiate a STOP condition.
				(0<<TWWC);										//
			break;
		case TWI_ARB_LOST:			// Arbitration lost
			TWCR = (1<<TWEN)|									// TWI Interface enabled
				(1<<TWIE)|(1<<TWINT)|							// Enable TWI Interupt and clear the flag
				(0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|				// Initiate a (RE)START condition.
				(0<<TWWC);										//
			break;
		case TWI_MTX_ADR_NACK:		// SLA+W has been tramsmitted and NACK received
		case TWI_MRX_ADR_NACK:		// SLA+R has been tramsmitted and NACK received    
		case TWI_MTX_DATA_NACK:		// Data byte has been tramsmitted and NACK received
		//case TWI_NO_STATE			// No relevant state information available; TWINT = “0”
		case TWI_BUS_ERROR:			// Bus error due to an illegal START or STOP condition
		default:     
			_i2c_state = TWSR;									// Store TWSR and automatically sets clears noErrors bit.
																// Reset TWI Interface
			TWCR = (1<<TWEN)|									// Enable TWI-interface and release TWI pins
				(0<<TWIE)|(0<<TWINT)|							// Disable Interupt
				(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|				// No Signal requests
				(0<<TWWC);										//
  }
}

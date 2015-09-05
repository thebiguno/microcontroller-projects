#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define NO_ERROR							0

#define INCOMING_ERROR_UNEXPECTED_START_OF_FRAME	1
#define INCOMING_ERROR_INVALID_CHECKSUM				2
#define INCOMING_ERROR_INVALID_LENGTH				3

#define OUTGOING_ERROR_QUEUE_FULL					4

#define START 0x7e
#define ESCAPE 0x7d

#define MAX_SIZE 40

namespace digitalcave {

	class Message {
		private:
			//Data
			uint8_t command;
			uint8_t message[MAX_SIZE];
		
			//State
			uint8_t length;
			uint8_t position;
			uint8_t escapedByte;
			uint8_t committed;	//Once flush is called, the message is committed and cannot be written to any more
		
			//Wrap all outgoing bytes through this; if the byte needs to be escaped, then set the
			// flag and send an escape char instead.
			uint8_t escapeByte(uint8_t b);
		
		public:
			//Construct a new message for writing
			Message(uint8_t command);
		
			//Encapsulate an existing message for reading
			Message(uint8_t command, uint8_t* message, uint8_t length);
		
			uint8_t getCommand();
			uint8_t getLength();
			uint8_t* getMessage();
		
			//Construct the message one byte at a time
			void append(uint8_t b);
		
			//Call this repeatedly to serialize the message, one byte at a time, into the b* value.  Returns 1 if
			// this value is part of the serialization, and 0 otherwise.
			uint8_t flush(uint8_t* result);
	};

	class Protocol {
		private:
			//Incoming state
			uint8_t position;			// Current position in the frame
			uint8_t length;				// Frame length
			uint8_t command;					// Incoming message command
			uint8_t checksum;					// Checksum
			uint8_t escape;	 					// Escape byte seen, unescape next byte
			uint8_t error;	 					// Error condition, ignore bytes until next frame start byte
			uint8_t message[MAX_SIZE];	// Incoming message

		public:
			Protocol();

			/*
			 * Process the next incoming byte  This function MUST be called from the main code whenever a byte is received.
			 * If a message is completed with this call, then return the completed Message object.
			 */
			Message* poll(uint8_t b);
		
			/*
			 * Gets the latest error status code.  0 means no error, non-zero is error.
			 */
			uint8_t getError();
	};
}

#endif

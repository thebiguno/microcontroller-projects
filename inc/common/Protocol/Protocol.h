#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#include "../Stream/Stream.h"

#define NO_ERROR							0

#define INCOMING_ERROR_UNEXPECTED_START_OF_FRAME	1
#define INCOMING_ERROR_INVALID_CHECKSUM				2
#define INCOMING_ERROR_INVALID_LENGTH				3
#define INCOMING_ERROR_EXCEED_MAX_LENGTH			4

#define OUTGOING_ERROR_QUEUE_FULL					5

#define START 0x7e
#define ESCAPE 0x7d

#define MAX_SIZE 40

namespace digitalcave {

	class Message {
		private:
			//Data
			uint8_t command;
			uint8_t data[MAX_SIZE];
		
			//State
			uint8_t length;

		public:
			//Construct a new message for writing
			Message(uint8_t command);
		
			//Encapsulate an existing message for reading
			Message(uint8_t command, uint8_t* message, uint8_t length);
			
			//Copies all of the attributes from message m to this object
			void clone(Message* m);
		
			uint8_t getCommand();
			uint8_t getLength();
			uint8_t* getData();
		
			//Construct the message one byte at a time
			void append(uint8_t b);
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
			
			//Convenience method to escape the given byte if needed
			void escapeByte(Stream* stream, uint8_t b);

		public:
			Protocol();

			/*
			 * Process any available incoming bytes from the stream.  This function MUST be called from the main code repeatedly.
			 * If a message is completed with this call, then return 1 and update the message's internal values, otherwise return 0.
			 */
			uint8_t read(Stream* stream, Message* result);
			
			//Call this to write the entire message into the provided stream.
			void write(Stream* stream, Message* message);
		
			/*
			 * Gets the latest error status code.  0 means no error, non-zero is error.
			 */
			uint8_t getError();
	};
}

#endif

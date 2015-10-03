#ifndef FRAMED_SERIAL_PROTOCOL_H
#define FRAMED_SERIAL_PROTOCOL_H

#include <stdint.h>

#include "../Stream/Stream.h"

//Error codes
#define NO_ERROR									0
#define INCOMING_ERROR_UNEXPECTED_START_OF_FRAME	1
#define INCOMING_ERROR_INVALID_CHECKSUM				2
#define INCOMING_ERROR_INVALID_LENGTH				3
#define INCOMING_ERROR_EXCEED_MAX_LENGTH			4
#define OUTGOING_ERROR_QUEUE_FULL					5

//Special bytes
#define START 0x7e
#define ESCAPE 0x7d

namespace digitalcave {

	class FramedSerialMessage {
		private:
			//Data
			uint8_t command;
			uint8_t* data;
		
			//State
			uint8_t length;
			
			//Metadata
			uint8_t maxSize;	//Cannot be larger than length

		public:
			//Construct a new message for writing
			FramedSerialMessage(uint8_t command, uint8_t maxSize);
		
			//Encapsulate an existing message for reading
			FramedSerialMessage(uint8_t command, uint8_t* message, uint8_t length);
			
			~FramedSerialMessage();
			
			//Copies all of the attributes from message m to this object
			void clone(FramedSerialMessage* m);
		
			uint8_t getCommand();
			uint8_t getLength();
			uint8_t* getData();
		
			//Construct the message one byte at a time
			void append(uint8_t b);
	};

	class FramedSerialProtocol {
		private:
			//Incoming state
			uint8_t position;			// Current position in the frame
			uint8_t length;				// Frame length
			uint8_t command;			// Incoming message command
			uint8_t checksum;			// Checksum
			uint8_t escape;	 			// Escape byte seen, unescape next byte
			uint8_t error;	 			// Error condition, ignore bytes until next frame start byte
			uint8_t* data;				// Incoming message
			uint8_t maxSize;			// Data array size
			
			//Convenience method to escape the given byte if needed
			void escapeByte(Stream* stream, uint8_t b);

		public:
			FramedSerialProtocol(uint8_t maxSize);
			~FramedSerialProtocol();

			/*
			 * Process any available incoming bytes from the stream.  This function MUST be called from the main code repeatedly.
			 * If a message is completed with this call, then return 1 and update the message's internal values, otherwise return 0.
			 */
			uint8_t read(Stream* stream, FramedSerialMessage* result);
			
			//Call this to write the entire message into the provided stream.
			void write(Stream* stream, FramedSerialMessage* message);
		
			/*
			 * Gets the latest error status code.  0 means no error, non-zero is error.
			 */
			uint8_t getError();
	};
}

#endif

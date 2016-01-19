//
//  FramedSerialProtocol.swift
//  Chindii
//
//  Created by Warren Janssens on 2016-01-18.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import Foundation

class FramedSerialProtocol {
	//Error codes
	let NO_ERROR									= 0
	let INCOMING_ERROR_UNEXPECTED_START_OF_FRAME	= 1
	let INCOMING_ERROR_INVALID_CHECKSUM				= 2
	let INCOMING_ERROR_INVALID_LENGTH				= 3
	let INCOMING_ERROR_EXCEED_MAX_LENGTH			= 4
	let OUTGOING_ERROR_QUEUE_FULL					= 5
	
	//Special bytes
	let START : UInt8 = 0x7e
	let ESCAPE : UInt8 = 0x7d

	//Incoming state
	var position : Int			// Current position in the frame
	var length : Int			// Frame length
	var command : UInt8			// Incoming message command
	var checksum : UInt8		// Checksum
	var escape : Bool	 		// Escape byte seen, unescape next byte
	var error : Int				// Error condition, ignore bytes until next frame start byte
	var data : [UInt8]			// Incoming message
	
	init() {
		position = 0
		length = 0
		command = 0
		checksum = 0
		escape = false
		error = NO_ERROR
		data = [UInt8]()
	}
	
	//Convenience method to escape the given byte if needed
	func escapeByte(data : NSData, b : UInt8) {
		if (b == START || b == ESCAPE) {
			data.
			stream->write(ESCAPE);
			stream->write(b ^ 0x20);
		} else {
			stream->write(b);
		}
	}
	
	/*
	* Process any available incoming bytes from the stream.  This function MUST be called from the main code repeatedly.
	* If a message is completed with this call, then return 1 and update the message's internal values, otherwise return 0.
	*/
	func read(stream : NSData, result : FramedSerialMessage) -> Bool {
	
	}
	
	//Call this to write the entire message into the provided stream.
	func write(stream : NSData, message : FramedSerialMessage) {
		
	}
	
	/*
	* Gets the latest error status code.  0 means no error, non-zero is error.
	*/
	func getError() -> Int { return error }
};
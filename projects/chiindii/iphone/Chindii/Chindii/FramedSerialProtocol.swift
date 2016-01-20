//
//  FramedSerialProtocol.swift
//  Chindii
//
//  Created by Warren Janssens on 2016-01-18.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import Foundation
import CoreBluetooth

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
	func escapeByte(stream : PeripheralStream, b : UInt8) {
		if (b == START || b == ESCAPE) {
			stream.write(ESCAPE)
			stream.write(b ^ 0x20);
		} else {
			stream.write(b);
		}
	}
	
	/*
	* Process any available incoming bytes from the stream.  This function MUST be called from the main code repeatedly.
	* If a message is completed with this call, then return 1 and update the message's internal values, otherwise return 0.
	*/
	func read(stream : PeripheralStream, result : FramedSerialMessage) -> Bool {
		var b = stream.read()
		while (b != nil) {
			if (error > 0) {
				if (b == START) {
					// recover from any previous error condition
					error = NO_ERROR;
					position = 0;
				}
				else {
					continue;
				}
			}
			
			if (position > 0 && b == START) {
				// unexpected start of frame
				error = INCOMING_ERROR_UNEXPECTED_START_OF_FRAME;
				continue;
			}
			if (position > 0 && b == ESCAPE) {
				// unescape next byte
				escape = 1;
				continue;
			}
			if (escape) {
				// unescape current byte
				b = 0x20 ^ b;
				escape = 0;
			}
			if (position > 1) { // start byte and length byte not included in checksum
				checksum += b;
			}
			
			switch(position) {
			case 0: // start frame
				position++;
				break;
			case 1: // length
				if (b == 0){
					error = INCOMING_ERROR_INVALID_LENGTH;
				}
				else {
					length = b;
					position++;
				}
				break;
			case 2:
				command = b;
				position++;
				break;
			default:
				if ((position - 3) > maxSize){
					//Max size exceeded
					error = INCOMING_ERROR_EXCEED_MAX_LENGTH;
				}
				else if (position == (length + 2)) {
					if (checksum == 0xff) {
						FramedSerialMessage m(command, data, length - 1);
						result->clone(&m);
						position = 0;
						checksum = 0;
						return 1;
					} else {
						error = INCOMING_ERROR_INVALID_CHECKSUM;
					}
					position = 0;
					checksum = 0;
				}
				else {
					data[position - 3] = b;
					position++;
				}
				break;
			}
		}
		
		return 0;
	}
	
	//Call this to write the entire message into the provided stream.
	func write(stream : PeripheralStream, message : FramedSerialMessage) {
		let command = message.getCommand()
		let data = message.getData()
		let length = data.count;
		
		for (var position = 0; position <= (length + 3); position++) {
			switch(position){
			case 0:
				stream.write(START);
				break;
			case 1:
				escapeByte(stream, b: UInt8(length + 1));
				break;
			case 2:
				escapeByte(stream, b: command);
				break;
			default:
				if (position - 3 == length){
					//Write checksum
					var result = command;
					
					for var i = 0; i < length; i++ {
						result += data[i];
					}
					
					escapeByte(stream, b: 0xff - result);
				}
				else {
					escapeByte(stream, b: data[position - 3]);
				}
				break;
			}
		}
	}
	
	/*
	* Gets the latest error status code.  0 means no error, non-zero is error.
	*/
	func getError() -> Int { return error }
};
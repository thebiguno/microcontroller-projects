//
//  FramedSerialProtocol.swift
//  Chindii
//
//  Created by Warren Janssens on 2016-01-18.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import Foundation
import CoreBluetooth

protocol FramedSerialProtocolDelegate: NSObjectProtocol {
	func onMessage(message: FramedSerialMessage)
	func write(b: UInt8)
}

class FramedSerialProtocol : NSObject {
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
	var position : Int = 0			// Current position in the frame
	var length : UInt8 = 0			// Frame length
	var command : UInt8 = 0			// Incoming message command
	var checksum : UInt8 = 0		// Checksum
	var escape : Bool = false 		// Escape byte seen, unescape next byte
	var error : Int = 0				// Error condition, ignore bytes until next frame start byte
	var data = [UInt8]()			// Incoming message
	var delegate : FramedSerialProtocolDelegate!
	
	//Convenience method to escape the given byte if needed
	func escapeByte(b : UInt8) {
		if (b == START || b == ESCAPE) {
			delegate.write(ESCAPE)
			delegate.write(b ^ 0x20);
		} else {
			delegate.write(b);
		}
	}
	
	func onMessage(message: [UInt8]) {
		for var i = 0; i < message.count; i++ {
			var b = message[i]

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
				escape = true;
				continue;
			}
			if (escape) {
				// unescape current byte
				b = 0x20 ^ b;
				escape = false;
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
				if (position == Int(length + 2)) {
					if (checksum == 0xff) {
						delegate.onMessage(FramedSerialMessage(command : command, data : data))
						position = 0;
						checksum = 0;
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
	}
	
	//Call this to write the entire message into the provided stream.
	func write(message : FramedSerialMessage) {
		let command = message.getCommand()
		let data = message.getData()
		let length = data.count;
		
		for (var position = 0; position <= (length + 3); position++) {
			switch(position){
			case 0:
				delegate.write(START);
				break;
			case 1:
				escapeByte(UInt8(length + 1));
				break;
			case 2:
				escapeByte(command);
				break;
			default:
				if (position - 3 == length){
					//Write checksum
					var result = command;
					
					for var i = 0; i < length; i++ {
						result = result &+ data[i]
					}
					
					escapeByte(0xff - result);
				}
				else {
					escapeByte(data[position - 3]);
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
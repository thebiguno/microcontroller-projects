//
//  FramedSerialMessage.swift
//  Chindii
//
//  Created by Warren Janssens on 2016-01-18.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import Foundation

class FramedSerialMessage {
	//Data
	var command : UInt8;
	var data : [UInt8];
	
	// Construct a new message for writing
	init(command : UInt8) {
		self.command = command
		self.data = [];
	}
	
	// Encapsulate an existing message for reading
	init(command : UInt8, data : [UInt8]) {
		self.command = command
		self.data = data
	}
	
	//Copies all of the attributes from message m to this object
	init(m : FramedSerialMessage) {
		self.command = m.command
		self.data = m.data
	}
	
	func getCommand() -> UInt8 { return command }
	func getData() -> [UInt8] { return data }
	
	func append(b : UInt8) { data.append(b) }
}

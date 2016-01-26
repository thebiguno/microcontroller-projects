//
//  DirectMessageManager.swift
//  Chindii
//
//  Created by Warren Janssens on 2016-01-24.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import Foundation

class DirectMessageManager {
	
	let MESSAGE_ARMED : UInt8 = 0x20
	let MESSAGE_THROTTLE : UInt8 = 0x21
	let MESSAGE_RATE : UInt8 = 0x22
	let MESSAGE_ANGLE : UInt8 = 0x23
	let MESSAGE_LEVEL : UInt8 = 0x24
	
	var app : AppDelegate
	
	init(app: AppDelegate) {
		self.app = app;
	}
	
	func dispatch(message : FramedSerialMessage) {
		
	}

}
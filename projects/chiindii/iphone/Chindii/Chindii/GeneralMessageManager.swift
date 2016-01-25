//
//  GeneralMessageManager.swift
//  Chindii
//
//  Created by Warren Janssens on 2016-01-24.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import Foundation

class GeneralMessageManager {
	
	let MESSAGE_ANNOUNCE_CONTROL_ID : UInt8 = 0x00
	let MESSAGE_SEND_ACKNOWLEDGE : UInt8 = 0x01
	let MESSAGE_SEND_COMPLETE : UInt8 = 0x02
	let MESSAGE_REQUEST_ENABLE_DEBUG : UInt8 = 0x03
	let MESSAGE_REQUEST_DISABLE_DEBUG : UInt8 = 0x04
	let MESSAGE_SEND_DEBUG : UInt8 = 0x05
	let MESSAGE_REQUEST_BATTERY : UInt8 = 0x06
	let MESSAGE_SEND_BATTERY : UInt8 = 0x07

	var app : AppDelegate
	
	init(app: AppDelegate) {
		self.app = app;
	}
	
	func dispatch(message : FramedSerialMessage) {
		switch(message.command) {
		case MESSAGE_SEND_BATTERY:
			app.flight.battery = message.data[0]
		}
	}
}
//
//  CalibrationMessageManager.swift
//  Chindii
//
//  Created by Warren Janssens on 2016-01-24.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import Foundation

class CalibrationMessageManager {
	
	let MESSAGE_SAVE_CALIBRATION : UInt8 = 0x30
	let MESSAGE_LOAD_CALIBRATION : UInt8 =  0x31
	let MESSAGE_RESET_CALIBRATION : UInt8 =  0x32
	let MESSAGE_REQUEST_CALIBRATION_RATE_PID : UInt8 =  0x33
	let MESSAGE_SEND_CALIBRATION_RATE_PID : UInt8 =  0x34
	let MESSAGE_REQUEST_CALIBRATION_ANGLE_PID : UInt8 =  0x35
	let MESSAGE_SEND_CALIBRATION_ANGLE_PID : UInt8 =  0x36
	let MESSAGE_REQUEST_CALIBRATION_COMPLEMENTARY : UInt8 =  0x37
	let MESSAGE_SEND_CALIBRATION_COMPLEMENTARY : UInt8 =  0x38
	let MESSAGE_START_CALIBRATION_COMPLEMENTARY : UInt8 =  0x39
	let MESSAGE_SEND_TUNING_DATA : UInt8 =  0x3A

	var app : AppDelegate
	
	init(app: AppDelegate) {
		self.app = app;
	}
	
	func dispatch(message : FramedSerialMessage) {
		switch(message.command) {
		case MESSAGE_SEND_CALIBRATION_RATE_PID:
			app.config.xRateP = unpack(message.data.prefix(4))
		}
	}
	
	func pack(var value: Float) -> [UInt8] {
		let valueByteArray = withUnsafePointer(&value) {
			Array(UnsafeBufferPointer(start: UnsafePointer<UInt8>($0), count: sizeofValue(value)))
		}
		return valueByteArray
	}
	
	func unpack(bytes: [UInt8]) -> Float {
		return bytes.withUnsafeBufferPointer {
			return UnsafePointer($0.baseAddress).memory
		}
	}
}
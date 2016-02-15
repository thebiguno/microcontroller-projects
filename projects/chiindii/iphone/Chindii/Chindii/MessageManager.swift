//
//  MessageManager.swift
//  Chindii
//
//  Created by Warren Janssens on 2016-02-02.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import Foundation
import CoreBluetooth

let sharedMessageManager = MessageManager()

class MessageManager : NSObject, FramedSerialProtocolDelegate, CBPeripheralDelegate {
	let MESSAGE_REQUEST_ENABLE_DEBUG : UInt8 = 0x03
	let MESSAGE_REQUEST_DISABLE_DEBUG : UInt8 = 0x04
	let MESSAGE_SEND_DEBUG : UInt8 = 0x05
	let MESSAGE_REQUEST_BATTERY : UInt8 = 0x06
	let MESSAGE_SEND_BATTERY : UInt8 = 0x07

	let MESSAGE_ARMED : UInt8 = 0x20
	let MESSAGE_THROTTLE : UInt8 = 0x21
	let MESSAGE_RATE : UInt8 = 0x22
	let MESSAGE_ANGLE : UInt8 = 0x23

	let MESSAGE_SAVE_CALIBRATION : UInt8 = 0x30
	let MESSAGE_LOAD_CALIBRATION : UInt8 =  0x31
	let MESSAGE_REQUEST_CALIBRATION_RATE_PID : UInt8 =  0x33
	let MESSAGE_SEND_CALIBRATION_RATE_PID : UInt8 =  0x34
	let MESSAGE_REQUEST_CALIBRATION_ANGLE_PID : UInt8 =  0x35
	let MESSAGE_SEND_CALIBRATION_ANGLE_PID : UInt8 =  0x36
	let MESSAGE_REQUEST_CALIBRATION_COMPLEMENTARY : UInt8 =  0x37
	let MESSAGE_SEND_CALIBRATION_COMPLEMENTARY : UInt8 =  0x38
	let MESSAGE_START_CALIBRATION_COMPLEMENTARY : UInt8 =  0x39
	let MESSAGE_SEND_TUNING_DATA : UInt8 =  0x3A
	let MESSAGE_LEVEL : UInt8 =  0x3B

	var serialProtocol = FramedSerialProtocol()

	var centralManager = CBCentralManager()
	var connectedPeripheral : CBPeripheral?
	var characteristic : CBCharacteristic?
	var writeType : CBCharacteristicWriteType?

	override init() {
		super.init()
		serialProtocol.delegate = self;
	}
	
	func flight() {
		sendMessages([
			FramedSerialMessage(command: MESSAGE_ARMED, data: [ sharedModel.armed ? 0x01 : 0x00 ]),
			FramedSerialMessage(command: MESSAGE_ANGLE, data: pack(sharedModel.angleSp))
		]);
	}
	
	func level() {
		sendMessage(FramedSerialMessage(command: MESSAGE_LEVEL))
	}
	
	func rate() {
		sendMessages([
			FramedSerialMessage(command: MESSAGE_ARMED, data: [ 0x02 ]),
			FramedSerialMessage(command: MESSAGE_THROTTLE, data: pack(sharedModel.throttleSp / 100.0))
		])
		sendMessages([
			FramedSerialMessage(command: MESSAGE_RATE, data: pack(sharedModel.rateSp))
		])
	}
	
	func tuning() {
		sendMessages([
			FramedSerialMessage(command: MESSAGE_SEND_CALIBRATION_RATE_PID, data: pack(sharedModel.rateConfig)),
		])
		sendMessages([
			FramedSerialMessage(command: MESSAGE_SEND_CALIBRATION_ANGLE_PID, data: pack(sharedModel.angleConfig)),
		])
		sendMessages([
			FramedSerialMessage(command: MESSAGE_SEND_CALIBRATION_COMPLEMENTARY, data: pack(sharedModel.compConfig))
		])
	}
	
	func saveTuning() {
		sendMessage(FramedSerialMessage(command: MESSAGE_SAVE_CALIBRATION))
	}
	func revertTuning() {
		sendMessages([
			FramedSerialMessage(command: MESSAGE_LOAD_CALIBRATION),
			FramedSerialMessage(command: MESSAGE_REQUEST_CALIBRATION_RATE_PID),
			FramedSerialMessage(command: MESSAGE_REQUEST_CALIBRATION_ANGLE_PID),
			FramedSerialMessage(command: MESSAGE_REQUEST_CALIBRATION_COMPLEMENTARY),
		])
	}
	
	func onMessage(message: FramedSerialMessage) {
		switch(message.command) {
		case MESSAGE_SEND_BATTERY:
			sharedModel.battery = message.data[0]
			break;
		case MESSAGE_SEND_CALIBRATION_RATE_PID:
			sharedModel.rateConfig = unpack(message.data)
			break
		case MESSAGE_SEND_CALIBRATION_ANGLE_PID:
			sharedModel.angleConfig = unpack(message.data)
			break
		case MESSAGE_SEND_CALIBRATION_COMPLEMENTARY:
			sharedModel.compConfig = unpack(message.data)
			break
		default:
			break;
		}
	}
	
	func sendMessage(message : FramedSerialMessage) {
		sendMessages([message]);
	}
	func sendMessages(messages : [FramedSerialMessage]) {
		serialProtocol.send(messages)
	}
	
	func write(b : [UInt8]) {
		if (connectedPeripheral == nil) { return }
		let data = NSData(bytes: b, length: b.count)
		connectedPeripheral!.writeValue(data, forCharacteristic: characteristic!, type: .WithResponse)
	}
	
	func peripheral(peripheral: CBPeripheral, didDiscoverServices error: NSError?) {
		// discover all characteristics for all services
		peripheral.services
		for service in peripheral.services! {
			peripheral.discoverCharacteristics(nil, forService: service)
		}
	}
	
	func peripheral(peripheral: CBPeripheral, didDiscoverCharacteristicsForService service: CBService, error: NSError?) {
		// check whether the characteristic we're looking for (0xFFE1) is present
		for characteristic in service.characteristics! {
			
			if characteristic.UUID == CBUUID(string: "FFE1") {
				connectedPeripheral = peripheral
				self.characteristic = characteristic
				// subscribe to this value (so we'll get notified when there is serial data for us..)
				peripheral.setNotifyValue(true, forCharacteristic: characteristic)
			}
		}
	}
	
	func peripheral(peripheral: CBPeripheral, didUpdateValueForCharacteristic characteristic: CBCharacteristic, error: NSError?) {
		var buffer = [UInt8]()
		characteristic.value?.getBytes(&buffer, length: (characteristic.value?.length)!)
		serialProtocol.onMessage(buffer)
	}
	
	func pack(var value : Float) -> [UInt8] {
		let valueByteArray = withUnsafePointer(&value) {
			Array(UnsafeBufferPointer(start: UnsafePointer<UInt8>($0), count: 4))
		}
		return valueByteArray
	}
	
	func pack(vector : Vector) -> [UInt8] {
		var result = [UInt8]()
		result.appendContentsOf(pack(vector.x));
		result.appendContentsOf(pack(vector.y));
		result.appendContentsOf(pack(vector.z));
		return result;
	}
	
	func pack(pid : PID) -> [UInt8] {
		var result = [UInt8]()
		result.appendContentsOf(pack(pid.p));
		result.appendContentsOf(pack(pid.i));
		result.appendContentsOf(pack(pid.d));
		return result;
	}

	func pack(config : RateConfig) -> [UInt8] {
		var result = [UInt8]()
		result.appendContentsOf(pack(config.x));
		result.appendContentsOf(pack(config.y));
		result.appendContentsOf(pack(config.z));
		return result;
	}

	func pack(config : AngleConfig) -> [UInt8] {
		var result = [UInt8]()
		result.appendContentsOf(pack(config.x));
		result.appendContentsOf(pack(config.y));
		result.appendContentsOf(pack(config.z));
		return result;
	}

	func pack(config : CompConfig) -> [UInt8] {
		var result = [UInt8]()
		result.appendContentsOf(pack(config.x));
		result.appendContentsOf(pack(config.y));
		return result;
	}

	func unpack<T>(bytes: [UInt8]) -> T {
		let pointer = UnsafeMutablePointer<T>.alloc(sizeof(T.Type))
		let data = NSData(bytes: bytes, length: bytes.count)
		data.getBytes(pointer, length: sizeof(T.Type))
		return pointer.move()
	}

}

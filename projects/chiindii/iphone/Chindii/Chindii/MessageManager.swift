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

	var connectedPeripheral : CBPeripheral?
	var characteristic : CBCharacteristic?
	var writeType : CBCharacteristicWriteType?

	override init() {
		super.init()
		serialProtocol.delegate = self;
	}
	
	func flight() {
		sendMessage(FramedSerialMessage(command: MESSAGE_ARMED, data: [ sharedFlightModel.armed ? 0x01 : 0x00 ]))
		sendMessage(FramedSerialMessage(command: MESSAGE_ANGLE, data: pack(sharedFlightModel.angle)));
	}
	
	func level() {
		sendMessage(FramedSerialMessage(command: MESSAGE_LEVEL))
	}
	
	func rate() {
		sendMessage(FramedSerialMessage(command: MESSAGE_ARMED, data: [ 0x02 ]))
		sendMessage(FramedSerialMessage(command: MESSAGE_THROTTLE, data: pack(sharedConfigModel.throttle)))
		sendMessage(FramedSerialMessage(command: MESSAGE_RATE, data: pack(sharedConfigModel.rate)))
	}
	
	func rateTuning(rate : RateConfig) {
		sendMessage(FramedSerialMessage(command: MESSAGE_SEND_CALIBRATION_RATE_PID, data: pack(rate)))
	}
	func angleTuning(angle : AngleConfig) {
		sendMessage(FramedSerialMessage(command: MESSAGE_SEND_CALIBRATION_ANGLE_PID, data: pack(rate)))
	}
	func compTuning(comp: CompConfig) {
		sendMessage(FramedSerialMessage(command: MESSAGE_SEND_CALIBRATION_COMPLEMENTARY, data: pack(comp)))
	}
	
	func saveTuning() {
		sendMessage(FramedSerialMessage(command: MESSAGE_SAVE_CALIBRATION))
	}
	func revertTuning() {
		sendMessage(FramedSerialMessage(command: MESSAGE_LOAD_CALIBRATION))
		sendMessage(FramedSerialMessage(command: MESSAGE_REQUEST_CALIBRATION_RATE_PID))
		sendMessage(FramedSerialMessage(command: MESSAGE_REQUEST_CALIBRATION_ANGLE_PID))
		sendMessage(FramedSerialMessage(command: MESSAGE_REQUEST_CALIBRATION_COMPLEMENTARY))
	}
	
	func onMessage(message: FramedSerialMessage) {
		switch(message.command) {
		case MESSAGE_SEND_BATTERY:
			sharedFlightModel.battery = message.data[0]
			break;
		case MESSAGE_SEND_CALIBRATION_RATE_PID:
			sharedConfigModel.rate = unpack(message.data)
			break
		case MESSAGE_SEND_CALIBRATION_ANGLE_PID:
			sharedConfigModel.angle = unpack(message.data)
			break
		case MESSAGE_SEND_CALIBRATION_COMPLEMENTARY:
			sharedConfigModel.comp = unpack(message.data)
			break
		default:
			break;
		}
	}
	
	func sendMessage(message : FramedSerialMessage) {
		serialProtocol.write(message);
	}
	
	func write(b : UInt8) {
		let data = NSData(bytes: [b], length: 1)
		connectedPeripheral!.writeValue(data, forCharacteristic: characteristic!, type: writeType!)
	}
	
	func peripheral(peripheral: CBPeripheral, didDiscoverServices error: NSError?) {
		// discover all characteristics for all services
		for service in peripheral.services! {
			peripheral.discoverCharacteristics(nil, forService: service)
		}
	}
	
	func peripheral(peripheral: CBPeripheral, didDiscoverCharacteristicsForService service: CBService, error: NSError?) {
		// check whether the characteristic we're looking for (0xFFE1) is present
		for characteristic in service.characteristics! {
			if characteristic.UUID == CBUUID(string: "FFE1") {
				connectedPeripheral = peripheral
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
	
	
	func pack(var value: Any) -> [UInt8] {
		let valueByteArray = withUnsafePointer(&value) {
			Array(UnsafeBufferPointer(start: UnsafePointer<UInt8>($0), count: sizeofValue(value)))
		}
		return valueByteArray
	}
	
	func unpack<T>(bytes: [UInt8]) -> T {
		let pointer = UnsafeMutablePointer<T>.alloc(sizeof(T.Type))
		let data = NSData(bytes: bytes, length: bytes.count)
		data.getBytes(pointer, length: sizeof(T.Type))
		return pointer.move()
	}

}

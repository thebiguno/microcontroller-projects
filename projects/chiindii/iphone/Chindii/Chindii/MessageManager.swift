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

extension Array {
	func slice(args: Int...) -> Array {
		var s = args[0]
		var e = self.count - 1
		if args.count > 1 { e = args[1] }
		
		if e < 0 {
			e += self.count
		}
		
		if s < 0 {
			s += self.count
		}
		
		let count = (s < e ? e-s : s-e)+1
		let inc = s < e ? 1 : -1
		var ret = Array()
		
		var idx = s
		for var i=0;i<count;i++  {
			ret.append(self[idx])
			idx += inc
		}
		return ret
	}
}

class MessageManager : NSObject, FramedSerialProtocolDelegate, CBCentralManagerDelegate, CBPeripheralDelegate {
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
	
	var timer : NSTimer?
	
	override init() {
		super.init()
		serialProtocol.delegate = self;
		centralManager.delegate = self;
	}
	
	func arm() {
		timer = NSTimer.scheduledTimerWithTimeInterval(0.2, target: self, selector: "flight", userInfo: nil, repeats: true)
	}
	func disarm() {
		if (timer != nil) {
			timer!.invalidate()
			timer = nil;
		}
	}
	
	func flight() {
		let c = Float(M_PI) / 180.0
		var angle = Vector()
		angle.x = sharedModel.angleSp.x * c
		angle.y = sharedModel.angleSp.y * c
		angle.z = sharedModel.angleSp.z * c

		sendMessages([
			FramedSerialMessage(command: MESSAGE_ARMED, data: [ sharedModel.armed ? 0x01 : 0x00 ]),
			FramedSerialMessage(command: MESSAGE_ANGLE, data: pack(sharedModel.angleSp)),
			FramedSerialMessage(command: MESSAGE_REQUEST_BATTERY)
		]);
	}
	
	func level() {
		sendMessage(FramedSerialMessage(command: MESSAGE_LEVEL))
	}
	
	func rate() {
		let c = Float(M_PI) / 180.0
		var rate = Vector()
		rate.x = sharedModel.rateSp.x * c
		rate.y = sharedModel.rateSp.y * c
		rate.z = sharedModel.rateSp.z * c
		
		sendMessages([
			FramedSerialMessage(command: MESSAGE_ARMED, data: [ 0x02 ]),
			FramedSerialMessage(command: MESSAGE_THROTTLE, data: pack(sharedModel.throttleSp / 100.0))
		])
		sendMessages([
			FramedSerialMessage(command: MESSAGE_RATE, data: pack(rate))
		])
	}

	func angle() {
		let c = Float(M_PI) / 180.0
		var angle = Vector()
		angle.x = sharedModel.angleSp.x * c
		angle.y = sharedModel.angleSp.y * c
		angle.z = sharedModel.angleSp.z * c
		
		sendMessages([
			FramedSerialMessage(command: MESSAGE_ARMED, data: [ 0x01 ]),
			FramedSerialMessage(command: MESSAGE_THROTTLE, data: pack(sharedModel.throttleSp / 100.0))
		])
		sendMessages([
			FramedSerialMessage(command: MESSAGE_RATE, data: pack(angle))
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
			FramedSerialMessage(command: MESSAGE_LOAD_CALIBRATION)
		])
		sendMessages([
			FramedSerialMessage(command: MESSAGE_REQUEST_CALIBRATION_RATE_PID)
		])
		sendMessages([
			FramedSerialMessage(command: MESSAGE_REQUEST_CALIBRATION_ANGLE_PID)
		])
		sendMessages([
			FramedSerialMessage(command: MESSAGE_REQUEST_CALIBRATION_COMPLEMENTARY)
		])
	}
	
	func onMessage(message: FramedSerialMessage) {
		print(message.command)
		print(message.data)
	
		switch(message.command) {
		case MESSAGE_SEND_BATTERY:
			sharedModel.battery = message.data[0]
			break;
		case MESSAGE_REQUEST_CALIBRATION_RATE_PID:
			sharedModel.rateConfig.x.p = unpack(message.data.slice(0,3))
			sharedModel.rateConfig.x.i = unpack(message.data.slice(4,7))
			sharedModel.rateConfig.x.d = unpack(message.data.slice(8,11))

			sharedModel.rateConfig.y.p = unpack(message.data.slice(12,15))
			sharedModel.rateConfig.y.i = unpack(message.data.slice(16,19))
			sharedModel.rateConfig.y.d = unpack(message.data.slice(20,23))

			sharedModel.rateConfig.z.p = unpack(message.data.slice(24,27))
			sharedModel.rateConfig.z.i = unpack(message.data.slice(28,31))
			sharedModel.rateConfig.z.d = unpack(message.data.slice(32,35))

			print("rate pid")
			print(sharedModel.rateConfig.x)
			print(sharedModel.rateConfig.y)
			print(sharedModel.rateConfig.z)
			break
		case MESSAGE_REQUEST_CALIBRATION_ANGLE_PID:
			sharedModel.angleConfig = unpack(message.data)
			sharedModel.angleConfig.x.p = unpack(message.data.slice(0,3))
			sharedModel.angleConfig.x.i = unpack(message.data.slice(4,7))
			sharedModel.angleConfig.x.d = unpack(message.data.slice(8,11))
			
			sharedModel.angleConfig.y.p = unpack(message.data.slice(12,15))
			sharedModel.angleConfig.y.i = unpack(message.data.slice(16,19))
			sharedModel.angleConfig.y.d = unpack(message.data.slice(20,23))
			
			sharedModel.angleConfig.z.p = unpack(message.data.slice(24,27))
			sharedModel.angleConfig.z.i = unpack(message.data.slice(28,31))
			sharedModel.angleConfig.z.d = unpack(message.data.slice(32,35))

			print("angle pid")
			print(sharedModel.angleConfig.x)
			print(sharedModel.angleConfig.y)
			print(sharedModel.angleConfig.z)
			break
		case MESSAGE_REQUEST_CALIBRATION_COMPLEMENTARY:
			sharedModel.compConfig = unpack(message.data)
			sharedModel.compConfig.x = unpack(message.data.slice(0,3))
			sharedModel.compConfig.y = unpack(message.data.slice(4,7))

			print("comp")
			print(sharedModel.compConfig)
			break
		case MESSAGE_SEND_DEBUG:
			print("debug")
			let debug = NSString(bytes: message.data, length: message.data.count, encoding: NSASCIIStringEncoding)
			print(debug)
			sharedModel.debug = debug as! String
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
	
	func centralManager(central: CBCentralManager, didDiscoverPeripheral peripheral: CBPeripheral, advertisementData: [String : AnyObject], RSSI: NSNumber) {
		sharedModel.peripherals.append(peripheral)
	}
	
	func centralManager(central: CBCentralManager, didConnectPeripheral peripheral: CBPeripheral) {
		peripheral.discoverServices([CBUUID(string: "FFE0")])
	}
	
	func centralManager(central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: NSError?) {
		print("disconnected")
		
	}
	
	func centralManager(central: CBCentralManager, didFailToConnectPeripheral peripheral: CBPeripheral, error: NSError?) {
		print("failed to connect")
	}
	
	func centralManagerDidUpdateState(central: CBCentralManager) {
		sharedModel.peripherals.removeAll();
		
		if (connectedPeripheral != nil) {
			centralManager.cancelPeripheralConnection(connectedPeripheral!)
		}
		if (centralManager.state == .PoweredOn) {
			centralManager.scanForPeripheralsWithServices([CBUUID(string: "FFE0")], options: nil)
		}
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
		if (error != nil) {
			print("Error reading characteristic \(error!.localizedDescription)")
		}
		let value = characteristic.value
		let length = value?.length
		//print(length!)
		//print("Characteristic value : \(characteristic.value) with ID \(characteristic.UUID)");
		
		var buffer = [UInt8](count: length!, repeatedValue: 0)
		value?.getBytes(&buffer, length: length!)
		//print(buffer)
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

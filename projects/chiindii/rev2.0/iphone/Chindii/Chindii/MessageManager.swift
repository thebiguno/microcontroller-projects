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
	let MESSAGE_BATTERY : UInt8 = 0x01
	let MESSAGE_STATUS : UInt8 = 0x02
	let MESSAGE_DEBUG : UInt8 = 0x03

	let MESSAGE_ARMED : UInt8 = 0x20
	let MESSAGE_THROTTLE : UInt8 = 0x21
	let MESSAGE_RATE : UInt8 = 0x22
	let MESSAGE_ANGLE : UInt8 = 0x23

	let MESSAGE_SAVE_CALIBRATION : UInt8 = 0x30
	let MESSAGE_LOAD_CALIBRATION : UInt8 =  0x31
	let MESSAGE_CALIBRATE_IMU : UInt8 = 0x32
	let MESSAGE_MADGWICK_TUNING : UInt8 = 0x33
	let MESSAGE_RATE_PID_TUNING : UInt8 =  0x34
	let MESSAGE_ANGLE_PID_TUNING : UInt8 =  0x35
	let MESSAGE_THROTTLE_PID_TUNING : UInt8 =  0x36

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
	
	func armFlight() {
		flight()
		sendMessage(FramedSerialMessage(command: MESSAGE_ARMED, data: [ 0x01 ]))
		timer = NSTimer.scheduledTimerWithTimeInterval(0.2, target: self, selector: "flight", userInfo: nil, repeats: true)
	}
	func armRate() {
		rate()
		sendMessage(FramedSerialMessage(command: MESSAGE_ARMED, data: [ 0x02 ]))
		timer = NSTimer.scheduledTimerWithTimeInterval(0.2, target: self, selector: "rate", userInfo: nil, repeats: true)
	}
	func armAngle() {
		angle()
		sendMessage(FramedSerialMessage(command: MESSAGE_ARMED, data: [ 0x03 ]))
		timer = NSTimer.scheduledTimerWithTimeInterval(0.2, target: self, selector: "angle", userInfo: nil, repeats: true)
	}
	func disarm() {
		if (timer != nil) {
			timer!.invalidate()
			timer = nil;
		}
		sendMessage(FramedSerialMessage(command: MESSAGE_ARMED, data: [ 0x00 ]))
	}
	
	func flight() {
		let c = Float(M_PI) / 180.0
		var angle = Vector()
		angle.x = sharedModel.angleSp.x * c
		angle.y = sharedModel.angleSp.y * c
		angle.z = sharedModel.angleSp.z * c

		sendMessages([
			FramedSerialMessage(command: MESSAGE_ANGLE, data: pack(sharedModel.angleSp)),
			FramedSerialMessage(command: MESSAGE_THROTTLE, data: pack(sharedModel.throttleSp)), // this is a g-force value
			FramedSerialMessage(command: MESSAGE_BATTERY)
		])
	}
	
	func calibrate() {
		sendMessage(FramedSerialMessage(command: MESSAGE_CALIBRATE_IMU))
	}
	
	func rate() {
		let c = Float(M_PI) / 180.0
		var rate = Vector()
		rate.x = sharedModel.rateSp.x * c
		rate.y = sharedModel.rateSp.y * c
		rate.z = sharedModel.rateSp.z * c
		
		sendMessages([
			FramedSerialMessage(command: MESSAGE_RATE, data: pack(rate)),
			FramedSerialMessage(command: MESSAGE_THROTTLE, data: pack(sharedModel.throttleSp / 100.0)),
			FramedSerialMessage(command: MESSAGE_BATTERY),
		])
	}

	func angle() {
		let c = Float(M_PI) / 180.0
		var angle = Vector()
		angle.x = sharedModel.angleSp.x * c
		angle.y = sharedModel.angleSp.y * c
		angle.z = sharedModel.angleSp.z * c
		
		sendMessages([
			FramedSerialMessage(command: MESSAGE_ANGLE, data: pack(angle)),
			FramedSerialMessage(command: MESSAGE_THROTTLE, data: pack(sharedModel.throttleSp / 100.0)),
			FramedSerialMessage(command: MESSAGE_BATTERY)
		])
	}
	
	func tuning() {
		sendMessage(FramedSerialMessage(command: MESSAGE_RATE_PID_TUNING, data: pack(sharedModel.rateConfig)))
		sendMessage(FramedSerialMessage(command: MESSAGE_ANGLE_PID_TUNING, data: pack(sharedModel.angleConfig)))
		sendMessage(FramedSerialMessage(command: MESSAGE_THROTTLE_PID_TUNING, data: pack(sharedModel.gforceConfig)))
		sendMessage(FramedSerialMessage(command: MESSAGE_MADGWICK_TUNING, data: pack(sharedModel.madgwickConfig)))
	}
	
	func saveTuning() {
		sendMessage(FramedSerialMessage(command: MESSAGE_SAVE_CALIBRATION))
	}
	func revertTuning() {
		sendMessage(FramedSerialMessage(command: MESSAGE_LOAD_CALIBRATION))
		sendMessage(FramedSerialMessage(command: MESSAGE_MADGWICK_TUNING))
		sendMessage(FramedSerialMessage(command: MESSAGE_RATE_PID_TUNING))
		sendMessage(FramedSerialMessage(command: MESSAGE_ANGLE_PID_TUNING))
		sendMessage(FramedSerialMessage(command: MESSAGE_THROTTLE_PID_TUNING))
	}
	
	func onMessage(message: FramedSerialMessage) {
		print(message.command)
		print(message.data)
	
		switch(message.command) {
		case MESSAGE_BATTERY:
			sharedModel.battery = message.data[0]
			break;
		case MESSAGE_RATE_PID_TUNING:
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
		case MESSAGE_ANGLE_PID_TUNING:
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
		case MESSAGE_THROTTLE_PID_TUNING:
			sharedModel.gforceConfig.p = unpack(message.data.slice(0,3))
			sharedModel.gforceConfig.i = unpack(message.data.slice(4,7))
			sharedModel.gforceConfig.d = unpack(message.data.slice(8,11))
			
			print("gforce pid")
			print(sharedModel.gforceConfig)
			break
		case MESSAGE_MADGWICK_TUNING:
			sharedModel.madgwickConfig = unpack(message.data)

			print("madgwick")
			print(sharedModel.madgwickConfig)
			break
		case MESSAGE_STATUS:
			print("status")
			let status = NSString(bytes: message.data, length: message.data.count, encoding: NSASCIIStringEncoding)
			print(status)
			sharedModel.status = status as! String
		case MESSAGE_DEBUG:
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

	func unpack<T>(bytes: [UInt8]) -> T {
		let pointer = UnsafeMutablePointer<T>.alloc(sizeof(T.Type))
		let data = NSData(bytes: bytes, length: bytes.count)
		data.getBytes(pointer, length: sizeof(T.Type))
		return pointer.move()
	}

}

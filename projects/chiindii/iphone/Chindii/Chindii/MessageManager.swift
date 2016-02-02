//
//  MessageManager.swift
//  Chindii
//
//  Created by Warren Janssens on 2016-02-02.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import Foundation
import CoreBluetooth

let instance = MessageManager()

class MessageManager : NSObject, FramedSerialProtocolDelegate, CBPeripheralDelegate {
	var serialProtocol = FramedSerialProtocol()

	var connectedPeripheral : CBPeripheral?
	var characteristic : CBCharacteristic?
	var writeType : CBCharacteristicWriteType?

	var general: GeneralMessageManager!
	var direct: DirectMessageManager!
	var calibration: CalibrationMessageManager!

	override init() {
		super.init()
		serialProtocol.delegate = self;
	}
	
	func onMessage(message: FramedSerialMessage) {
		if ((message.command & 0xF0) == 0x00){
			general.dispatch(message);
		}
		else if ((message.command & 0xF0) == 0x20){
			direct.dispatch(message);
		}
		else if ( (message.command & 0xF0) == 0x30){
			calibration.dispatch(message);
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

}

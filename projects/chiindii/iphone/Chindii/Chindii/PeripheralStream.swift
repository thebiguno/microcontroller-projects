//
//  PeripheralStream.swift
//  Chindii
//
//  Created by Warren Janssens on 2016-01-19.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import Foundation
import CoreBluetooth

class PeripheralStream : NSObject, CBPeripheralDelegate {
	var connectedPeripheral : CBPeripheral
	var characteristic : CBCharacteristic
	var writeType : CBCharacteristicWriteType
	var buffer : [UInt8] = []
	
	override init() {
		super.init()
	}
	
	func write(b : UInt8) {
		let data = NSData(bytes: [b], length: 1)
		connectedPeripheral.writeValue(data, forCharacteristic: characteristic, type: writeType)
	}
	
	func read() -> UInt8 {
		return buffer.removeFirst()
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
		characteristic.value?.getBytes(&buffer, length: (characteristic.value?.length)!)
	}
	
}
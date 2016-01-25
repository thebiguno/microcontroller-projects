//
//  PeripheralManager.swift
//  Chindii
//
//  Created by Warren Janssens on 2016-01-24.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import Foundation
import CoreBluetooth

class PeripheralManager: NSObject, CBCentralManagerDelegate, CBPeripheralDelegate {
	
	var stream : PeripheralStream
	var centralManager : CBCentralManager
	
	init(stream : PeripheralStream) {
		self.stream = stream;
		self.centralManager = CBCentralManager()
		super.init()
		self.centralManager.delegate = self;
	}
	
	func startScan() {
		centralManager.scanForPeripheralsWithServices(nil, options: nil)
	}
	func stopScan() {
		centralManager.stopScan()
	}

	func centralManager(central: CBCentralManager, didDiscoverPeripheral peripheral: CBPeripheral, advertisementData: [String : AnyObject], RSSI: NSNumber) {
		
	}
	
	func centralManager(central: CBCentralManager, didConnectPeripheral peripheral: CBPeripheral) {
		peripheral.delegate = stream
		peripheral.discoverServices(nil)
	}
	
	func centralManager(central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: NSError?) {
		
	}
	
	func centralManager(central: CBCentralManager, didFailToConnectPeripheral peripheral: CBPeripheral, error: NSError?) {
	}
	
	func centralManagerDidUpdateState(central: CBCentralManager) {
		
	}
}

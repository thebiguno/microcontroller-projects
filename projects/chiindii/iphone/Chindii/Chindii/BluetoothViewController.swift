//
//  BluetoothViewController.swift
//  Chindii
//
//  Created by Warren Janssens on 2016-01-30.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import UIKit
import CoreBluetooth

class BluetoothViewController : UITableViewController, CBCentralManagerDelegate {
	
	var centralManager : CBCentralManager!
	var peripherals = [CBPeripheral]()
	
	override func viewDidLoad() {
		centralManager = CBCentralManager(delegate: self, queue: nil)
	}
	
	override func didReceiveMemoryWarning() {
		super.didReceiveMemoryWarning()
	}
	
	override func viewWillAppear(animated: Bool) {

	}
	
	override func viewWillDisappear(animated: Bool) {
		centralManager.stopScan()
		peripherals.removeAll()
	}
	
	override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
		return peripherals.count
	}
	
	override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
		let peripheral = peripherals[indexPath.row];
		let cell = tableView.dequeueReusableCellWithIdentifier("peripheral")
		cell?.textLabel?.text = peripheral.name
		cell?.accessoryType = UITableViewCellAccessoryType.None
		return cell!;
	}
	
	override func tableView(tableView: UITableView, willSelectRowAtIndexPath indexPath: NSIndexPath) -> NSIndexPath? {
		let oldIndexPath = tableView.indexPathForSelectedRow
		if (oldIndexPath != nil) {
			let oldPeripheral = peripherals[oldIndexPath!.row];
			centralManager.cancelPeripheralConnection(oldPeripheral)
			tableView.cellForRowAtIndexPath(oldIndexPath!)?.accessoryType = UITableViewCellAccessoryType.None
		}
		tableView.cellForRowAtIndexPath(indexPath)?.accessoryType = UITableViewCellAccessoryType.Checkmark
		
		let peripheral = peripherals[indexPath.row]
		peripheral.delegate = sharedMessageManager
		centralManager.connectPeripheral(peripheral, options: nil)
		
		return indexPath
	}
	
	func centralManager(central: CBCentralManager, didDiscoverPeripheral peripheral: CBPeripheral, advertisementData: [String : AnyObject], RSSI: NSNumber) {
		peripherals.append(peripheral)
		tableView.reloadData()
	}
	
	func centralManager(central: CBCentralManager, didConnectPeripheral peripheral: CBPeripheral) {
		peripheral.discoverServices([CBUUID(string: "FFE0")])
	}
	
	func centralManager(central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: NSError?) {
		
	}
	
	func centralManager(central: CBCentralManager, didFailToConnectPeripheral peripheral: CBPeripheral, error: NSError?) {
	}
	
	func centralManagerDidUpdateState(central: CBCentralManager) {
		switch central.state {
		case .PoweredOn:
			centralManager.scanForPeripheralsWithServices([CBUUID(string: "FFE0")], options: nil)
			break
		default:
			break
		}
		
	}
	
	
}
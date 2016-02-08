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
	
	var peripherals = [CBPeripheral]()
//	var refreshControl : UIRefreshControl!
	
	override func viewDidLoad() {
		super.viewDidLoad()
		sharedMessageManager.centralManager.delegate = self;
		
		refreshControl!.addTarget(self, action: "refresh", forControlEvents: UIControlEvents.ValueChanged)
	}
	
	func refresh(sender: AnyObject) {
		refresh()
	}
	func refresh() {
		peripherals.removeAll();

		if (sharedMessageManager.connectedPeripheral != nil) {
			sharedMessageManager.centralManager.cancelPeripheralConnection(sharedMessageManager.connectedPeripheral!)
		}
		if (sharedMessageManager.centralManager.state == .PoweredOn) {
			sharedMessageManager.centralManager.scanForPeripheralsWithServices([CBUUID(string: "FFE0")], options: nil)
		}
	}
	
	override func didReceiveMemoryWarning() {
		super.didReceiveMemoryWarning()
	}
	
	override func viewWillAppear(animated: Bool) {
		if (sharedMessageManager.centralManager.state == .PoweredOn) {
			sharedMessageManager.centralManager.scanForPeripheralsWithServices([CBUUID(string: "FFE0")], options: nil)
		}
	}
	
	override func viewWillDisappear(animated: Bool) {
		sharedMessageManager.centralManager.stopScan()
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
			sharedMessageManager.centralManager.cancelPeripheralConnection(oldPeripheral)
			tableView.cellForRowAtIndexPath(oldIndexPath!)?.accessoryType = UITableViewCellAccessoryType.None
		}
		tableView.cellForRowAtIndexPath(indexPath)?.accessoryType = UITableViewCellAccessoryType.Checkmark
		
		let peripheral = peripherals[indexPath.row]
		peripheral.delegate = sharedMessageManager
		sharedMessageManager.centralManager.connectPeripheral(peripheral, options: nil)
		
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
		print("disconnected")

	}
	
	func centralManager(central: CBCentralManager, didFailToConnectPeripheral peripheral: CBPeripheral, error: NSError?) {
		print("failed to connect")
	}
	
	func centralManagerDidUpdateState(central: CBCentralManager) {
		refresh()
	}
	
	
}
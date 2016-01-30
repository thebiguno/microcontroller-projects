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
		centralManager = CBCentralManager()
		centralManager.delegate = self;
	}
	
	override func didReceiveMemoryWarning() {
		super.didReceiveMemoryWarning()
	}
	
	override func viewWillAppear(animated: Bool) {
		centralManager.scanForPeripheralsWithServices(nil, options: nil)
		
		let appDelegate = UIApplication.sharedApplication().delegate as! AppDelegate
	}
	
	override func viewWillDisappear(animated: Bool) {
		centralManager.stopScan()
	}
	
	override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
		return peripherals.count
	}
	
	override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
		let peripheral = peripherals[indexPath.row];
		let cell = tableView.dequeueReusableCellWithIdentifier("peripheral")
		cell?.textLabel?.text = peripheral.description
		return cell!;
	}
	
	func centralManager(central: CBCentralManager, didDiscoverPeripheral peripheral: CBPeripheral, advertisementData: [String : AnyObject], RSSI: NSNumber) {
		peripherals.append(peripheral)
		tableView.reloadData()
	}
	
	func centralManager(central: CBCentralManager, didConnectPeripheral peripheral: CBPeripheral) {
		peripheral.discoverServices([CBUUID(string: "FFE1")])
	}
	
	func centralManager(central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: NSError?) {
		
	}
	
	func centralManager(central: CBCentralManager, didFailToConnectPeripheral peripheral: CBPeripheral, error: NSError?) {
	}
	
	func centralManagerDidUpdateState(central: CBCentralManager) {
		
	}
	
	
}
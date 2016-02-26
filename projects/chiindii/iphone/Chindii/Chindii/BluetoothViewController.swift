//
//  BluetoothViewController.swift
//  Chindii
//
//  Created by Warren Janssens on 2016-01-30.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import UIKit
import CoreBluetooth

class BluetoothViewController : UITableViewController, ModelDelegate {
	
	override func viewDidLoad() {
		super.viewDidLoad()
		sharedModel.delegate = self;
		
		refreshControl!.addTarget(self, action: "refresh", forControlEvents: UIControlEvents.ValueChanged)
	}
	
	func batteryChanged() {}
	func configChanged() {}
	func debugChanged() {}
	func statusChanged() {}

	func peripheralsChanged() {
		tableView.reloadData()
	}
	
	func refresh(sender: AnyObject) {
		refresh()
	}
	func refresh() {
		sharedModel.peripherals.removeAll();

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
		return sharedModel.peripherals.count
	}
	
	override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
		let peripheral = sharedModel.peripherals[indexPath.row];
		let cell = tableView.dequeueReusableCellWithIdentifier("peripheral")
		cell?.textLabel?.text = peripheral.name
		cell?.accessoryType = UITableViewCellAccessoryType.None
		return cell!;
	}
	
	override func tableView(tableView: UITableView, willSelectRowAtIndexPath indexPath: NSIndexPath) -> NSIndexPath? {
		let oldIndexPath = tableView.indexPathForSelectedRow
		if (oldIndexPath != nil) {
			let oldPeripheral = sharedModel.peripherals[oldIndexPath!.row];
			sharedMessageManager.centralManager.cancelPeripheralConnection(oldPeripheral)
			tableView.cellForRowAtIndexPath(oldIndexPath!)?.accessoryType = UITableViewCellAccessoryType.None
		}
		tableView.cellForRowAtIndexPath(indexPath)?.accessoryType = UITableViewCellAccessoryType.Checkmark
		
		let peripheral = sharedModel.peripherals[indexPath.row]
		peripheral.delegate = sharedMessageManager
		sharedMessageManager.centralManager.connectPeripheral(peripheral, options: nil)
		
		performSegueWithIdentifier("unwindPeripherals", sender: self)
		
		return indexPath
	}
	
}
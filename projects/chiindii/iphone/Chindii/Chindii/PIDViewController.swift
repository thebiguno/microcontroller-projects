//
//  PIDViewController.swift
//  Chiindii
//
//  Created by Warren Janssens on 2016-02-25.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import UIKit

class PIDViewController : UITableViewController {
	
	let labels = [ "X Roll Rate", "Y Pitch Rate", "Z Yaw Rate", "X Roll Angle", "Y Pitch Angle", "Z Yaw Angle", "G-Force"]
	
	override func didReceiveMemoryWarning() {
		super.didReceiveMemoryWarning()
	}
	
	override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
		return labels.count
	}
	
	override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
		let cell = tableView.dequeueReusableCellWithIdentifier("pid")
		cell?.textLabel?.text = labels[indexPath.item]
		if (indexPath.item == sharedModel.selectedPID) {
			cell?.accessoryType = UITableViewCellAccessoryType.Checkmark
		} else {
			cell?.accessoryType = UITableViewCellAccessoryType.None
		}
		return cell!
	}
	
	override func tableView(tableView: UITableView, willSelectRowAtIndexPath indexPath: NSIndexPath) -> NSIndexPath? {
		let oldIndexPath = tableView.indexPathForSelectedRow
		if (oldIndexPath != nil) {
			tableView.cellForRowAtIndexPath(oldIndexPath!)?.accessoryType = UITableViewCellAccessoryType.None
		}
		tableView.cellForRowAtIndexPath(indexPath)?.accessoryType = UITableViewCellAccessoryType.Checkmark
		
		sharedModel.selectedPID = indexPath.item
		
		performSegueWithIdentifier("unwindPID", sender: self)
		
		return indexPath
	}
}

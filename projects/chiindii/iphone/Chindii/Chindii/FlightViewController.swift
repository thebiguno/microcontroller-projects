//
//  FlightViewController.swift
//  Chindii
//
//  Created by Warren Janssens on 2015-08-19.
//  Copyright (c) 2015 Warren Janssens. All rights reserved.
//

import UIKit

class FlightViewController: UIViewController, ModelDelegate {

	@IBOutlet var message : UILabel!
	@IBOutlet var armedSwitch : UISwitch!
	@IBOutlet var battery : UILabel!
	@IBOutlet var status : UILabel!
	@IBOutlet var debug : UILabel!
	
	@IBAction func armedPressed(sender : UISwitch) {
		if (sender.on) {
			sharedMessageManager.armFlight()
		} else {
			sharedMessageManager.disarm()
		}
	}
	
	@IBAction func levelPressed(sender : AnyObject) {
		sharedModel.angleSp.z = 0
	}
	
	@IBAction func upPressed(sender : AnyObject) {
		sharedModel.angleSp.z += 0.01
	}
	
	@IBAction func downPressed(sender : AnyObject) {
		sharedModel.angleSp.z -= 0.01
	}
	
	override func willRotateToInterfaceOrientation(toInterfaceOrientation: UIInterfaceOrientation, duration: NSTimeInterval) {
		if (toInterfaceOrientation.isPortrait) {
			performSegueWithIdentifier("toMenu", sender: self);
		}
	}
	
	override func viewDidLoad() {
        super.viewDidLoad()
	}
	
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
	
	override func viewWillAppear(animated: Bool) {
		sharedModel.delegate = self;
		battery.text = "\(sharedModel.battery)%"
		debug.text = ""
	}
	
	func batteryChanged() {
		battery.text = "\(sharedModel.battery)%"
	}
	
	func debugChanged() {
		debug.text = sharedModel.debug
	}
	
	func statusChanged() {
		status.text = sharedModel.status
	}

	func configChanged() {}
	func peripheralsChanged() {}

	
}

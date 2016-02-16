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
	
	var timer : NSTimer?
	
	@IBAction func armedPressed(sender : UISwitch) {
		sharedModel.armed = sender.on
		
		if (sender.on) {
			timer = NSTimer.scheduledTimerWithTimeInterval(0.2, target: sharedMessageManager, selector: "flight", userInfo: nil, repeats: true)
//			timer = NSTimer.init(timeInterval: 0.1, target: sharedMessageManager, selector: "flight", userInfo: nil, repeats: true)			
		} else {
			timer!.invalidate()
			timer = nil;
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
		armedSwitch.setOn(sharedModel.armed, animated: true)
		battery.text = "\(sharedModel.battery)%"
	}
	
	func batteryChanged() {
		battery.text = "\(sharedModel.battery)%"
	}
	
	func configChanged() {
	}
	
}

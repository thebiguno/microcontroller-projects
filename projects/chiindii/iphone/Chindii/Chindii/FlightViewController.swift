//
//  FlightViewController.swift
//  Chindii
//
//  Created by Warren Janssens on 2015-08-19.
//  Copyright (c) 2015 Warren Janssens. All rights reserved.
//

import UIKit

class FlightViewController: UIViewController {

	@IBOutlet var armedSwitch : UISwitch!
	@IBOutlet var battery : UIProgressView!
	
	var timer : NSTimer?
	
	@IBAction func armedPressed(sender : UISwitch) {
		sharedFlightModel.armed = sender.on
		
		if (sender.on) {
			timer = NSTimer.init(timeInterval: 0.1, target: sharedMessageManager, selector: "flight", userInfo: nil, repeats: true)
		} else {
			timer!.invalidate()
			timer = nil;
		}
	}
	
	@IBAction func levelPressed(sender : AnyObject) {
		sharedFlightModel.angle.z = 0
	}
	
	@IBAction func upPressed(sender : AnyObject) {
		sharedFlightModel.angle.z += 0.25
	}
	
	@IBAction func downPressed(sender : AnyObject) {
		sharedFlightModel.angle.z -= 0.25
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
	
}

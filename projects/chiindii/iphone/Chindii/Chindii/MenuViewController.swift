//
//  PortraitViewController.swift
//  Chindii
//
//  Created by Warren Janssens on 2015-08-19.
//  Copyright (c) 2015 Warren Janssens. All rights reserved.
//

import UIKit

class MenuViewController: UITableViewController, UITextFieldDelegate {
	
	@IBOutlet var throttle : UISlider!
	@IBOutlet var rate : UISlider!
	@IBOutlet var axis : UISegmentedControl!
	@IBOutlet var rateP : UITextField!
	@IBOutlet var rateI : UITextField!
	@IBOutlet var rateD : UITextField!
	@IBOutlet var angleP : UITextField!
	@IBOutlet var angleI : UITextField!
	@IBOutlet var angleD : UITextField!
	@IBOutlet var tau : UITextField!

	@IBAction func unwindToMenu(seque: UIStoryboardSegue) {
		
	}
	
	@IBAction func loadClicked(sender: AnyObject) {
		sharedMessageManager.revertTuning()
	}
	
	@IBAction func saveClicked(sender: AnyObject) {
		sharedMessageManager.saveTuning()
	}
	
	@IBAction func calibrateClicked(sender: AnyObject) {
		sharedMessageManager.level()
	}
	
	@IBAction func armClicked(sender: AnyObject) {
		sharedMessageManager.rate()
	}
	
	@IBAction func throttleChanged(sender : UISlider) {
		sender.value = roundf(sender.value)
		
		sharedConfigModel.throttle = sender.value * 10;
	}
	
	@IBAction func rateChanged(sender : UISlider) {
		sender.value = roundf(sender.value)
		sharedFlightModel.rate.x = 0;
		sharedFlightModel.rate.y = 0;
		sharedFlightModel.rate.z = 0;

		switch (axis.selectedSegmentIndex) {
		case 0:
			sharedFlightModel.rate.x = sender.value
			break;
		case 1:
			sharedFlightModel.rate.y = sender.value
			break;
		default:
			sharedFlightModel.rate.z = sender.value
			break;
		}
	}
	
	func textField(textField: UITextField, shouldChangeCharactersInRange range: NSRange, replacementString string: String) -> Bool {
		let updated = (textField.text! as NSString)
			.stringByReplacingCharactersInRange(range, withString: string);
		
		let numberFormatter = NSNumberFormatter()
		let number = numberFormatter.numberFromString(updated as String)
		if (updated == "") {
			return true;
		} else if (number == nil) {
			return false;
		} else {
			let numberFloatValue = number!.floatValue

			var ratePid : PID
			var anglePid : PID
			switch (axis.selectedSegmentIndex) {
			case 0:
				ratePid = sharedConfigModel.rate.x
				anglePid = sharedConfigModel.angle.x
				break;
			case 1:
				ratePid = sharedConfigModel.rate.y
				anglePid = sharedConfigModel.angle.y
				break;
			default:
				ratePid = sharedConfigModel.rate.z
				anglePid = sharedConfigModel.angle.z
				break;
			}
			if (textField == rateP) {
				ratePid.p = numberFloatValue
			}
			// TODO set this value into the model
			// TODO send this value to the peripheral
			return true;
		}
	}
	
	
	override func willRotateToInterfaceOrientation(toInterfaceOrientation: UIInterfaceOrientation, duration: NSTimeInterval) {
		if (toInterfaceOrientation.isLandscape) {
			performSegueWithIdentifier("toFlight", sender: self);
		}
	}
	
	override func viewDidLoad() {
		super.viewDidLoad()
		
		// Do any additional setup after loading the view, typically from a nib.
	}
	
	override func didReceiveMemoryWarning() {
		super.didReceiveMemoryWarning()
		// Dispose of any resources that can be recreated.
	}
	
}

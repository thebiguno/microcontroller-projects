//
//  PortraitViewController.swift
//  Chindii
//
//  Created by Warren Janssens on 2015-08-19.
//  Copyright (c) 2015 Warren Janssens. All rights reserved.
//

import UIKit

class MenuViewController: UITableViewController, UITextFieldDelegate, ModelDelegate {
	
	@IBOutlet var throttleLabel : UILabel!
	@IBOutlet var rateLabel : UILabel!
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
		
		sharedModel.throttleSp = sender.value * 10;
		throttleLabel.text = "\(sharedModel.throttleSp)%"
	}
	
	@IBAction func rateChanged(sender : UISlider) {
		sender.value = roundf(sender.value)
		sharedModel.rateSp.x = 0;
		sharedModel.rateSp.y = 0;
		sharedModel.rateSp.z = 0;

		switch (axis.selectedSegmentIndex) {
		case 0:
			sharedModel.rateSp.x = sender.value
			rateLabel.text = "\(sharedModel.rateSp.x)°/s"
			break;
		case 1:
			sharedModel.rateSp.y = sender.value
			rateLabel.text = "\(sharedModel.rateSp.y)°/s"
			break;
		default:
			sharedModel.rateSp.z = sender.value
			rateLabel.text = "\(sharedModel.rateSp.z)°/s"
			break;
		}
	}
	
	@IBAction func axisChanged(sender : UISegmentedControl) {
		update();
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

			switch (axis.selectedSegmentIndex) {
			case 0:
				if (textField == rateP)	{ sharedModel.rateConfig.x.p = numberFloatValue }
				else if (textField == rateI) { sharedModel.rateConfig.x.i = numberFloatValue }
				else if (textField == rateD) { sharedModel.rateConfig.x.d = numberFloatValue }
				else if (textField == angleP) { sharedModel.angleConfig.x.p = numberFloatValue }
				else if (textField == angleI) { sharedModel.angleConfig.x.i = numberFloatValue }
				else if (textField == angleD) { sharedModel.angleConfig.x.d = numberFloatValue }
				else if (textField == tau) { sharedModel.compConfig.x = numberFloatValue }
				break;
			case 1:
				if (textField == rateP)	{ sharedModel.rateConfig.y.p = numberFloatValue }
				else if (textField == rateI) { sharedModel.rateConfig.y.i = numberFloatValue }
				else if (textField == rateD) { sharedModel.rateConfig.y.d = numberFloatValue }
				else if (textField == angleP) { sharedModel.angleConfig.y.p = numberFloatValue }
				else if (textField == angleI) { sharedModel.angleConfig.y.i = numberFloatValue }
				else if (textField == angleD) { sharedModel.angleConfig.y.d = numberFloatValue }
				else if (textField == tau) { sharedModel.compConfig.y = numberFloatValue }
				break;
			default:
				if (textField == rateP)	{ sharedModel.rateConfig.z.p = numberFloatValue }
				else if (textField == rateI) { sharedModel.rateConfig.z.i = numberFloatValue }
				else if (textField == rateD) { sharedModel.rateConfig.z.d = numberFloatValue }
				else if (textField == angleP) { sharedModel.angleConfig.z.p = numberFloatValue }
				else if (textField == angleI) { sharedModel.angleConfig.z.i = numberFloatValue }
				else if (textField == angleD) { sharedModel.angleConfig.z.d = numberFloatValue }
				break;
			}
            sharedMessageManager.tuning()
			return false;
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
	
	override func viewWillAppear(animated: Bool) {
		super.viewWillAppear(animated);
		
		sharedModel.delegate = self;
		
		update();
	}
	
	func batteryChanged() {
	}
	
	func configChanged() {
		update();
	}
	
	func update() {
		throttle.value = sharedModel.throttleSp
		
		if (axis.selectedSegmentIndex == 0) {
			rate.value = sharedModel.rateSp.x
			rateP.text = "\(sharedModel.rateConfig.x.p)"
			rateI.text = "\(sharedModel.rateConfig.x.i)"
			rateD.text = "\(sharedModel.rateConfig.x.d)"
			angleP.text = "\(sharedModel.angleConfig.x.p)"
			angleI.text = "\(sharedModel.angleConfig.x.i)"
			angleD.text = "\(sharedModel.angleConfig.x.d)"
			tau.text = "\(sharedModel.compConfig.x)"
			rateLabel.text = "\(sharedModel.rateSp.x)°/s"
		} else if (axis.selectedSegmentIndex == 1) {
			rate.value = sharedModel.rateSp.y
			rateP.text = "\(sharedModel.rateConfig.y.p)"
			rateI.text = "\(sharedModel.rateConfig.y.i)"
			rateD.text = "\(sharedModel.rateConfig.y.d)"
			angleP.text = "\(sharedModel.angleConfig.y.p)"
			angleI.text = "\(sharedModel.angleConfig.y.i)"
			angleD.text = "\(sharedModel.angleConfig.y.d)"
			tau.text = "\(sharedModel.compConfig.y)"
			rateLabel.text = "\(sharedModel.rateSp.y)°/s"
		} else {
			rate.value = sharedModel.rateSp.x
			rateP.text = "\(sharedModel.rateConfig.z.p)"
			rateI.text = "\(sharedModel.rateConfig.z.i)"
			rateD.text = "\(sharedModel.rateConfig.z.d)"
			angleP.text = "\(sharedModel.angleConfig.z.p)"
			angleI.text = "\(sharedModel.angleConfig.z.i)"
			angleD.text = "\(sharedModel.angleConfig.z.d)"
			tau.text = "0"
			rateLabel.text = "\(sharedModel.rateSp.z)°/s"
		}
		
		throttleLabel.text = "\(sharedModel.throttleSp)%"

	}
	
}

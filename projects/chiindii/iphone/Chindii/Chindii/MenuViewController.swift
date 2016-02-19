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
	@IBOutlet var throttle : UISlider!
	@IBOutlet var rateLabel : UILabel!
	@IBOutlet var rate : UISlider!
	@IBOutlet var angleLabel : UILabel!
	@IBOutlet var angle : UISlider!
	@IBOutlet var axis : UISegmentedControl!
	@IBOutlet var rateP : UITextField!
	@IBOutlet var rateI : UITextField!
	@IBOutlet var rateD : UITextField!
	@IBOutlet var angleP : UITextField!
	@IBOutlet var angleI : UITextField!
	@IBOutlet var angleD : UITextField!
	@IBOutlet var tau : UITextField!
	
	var typing = false

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
	
	@IBAction func rateTestClicked(sender: AnyObject) {
		sharedMessageManager.rate()
	}
	
	@IBAction func angleTestClicked(sender: AnyObject) {
		sharedMessageManager.angle()
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
	
	@IBAction func angleChanged(sender : UISlider) {
		sender.value = roundf(sender.value)
		sharedModel.angleSp.x = 0;
		sharedModel.angleSp.y = 0;
		sharedModel.angleSp.z = 0;
		
		switch (axis.selectedSegmentIndex) {
		case 0:
			sharedModel.angleSp.x = sender.value
			angleLabel.text = "\(sharedModel.angleSp.x)°"
			break;
		case 1:
			sharedModel.angleSp.y = sender.value
			angleLabel.text = "\(sharedModel.angleSp.y)°"
			break;
		default:
			sharedModel.angleSp.z = sender.value
			angleLabel.text = "\(sharedModel.angleSp.z)°"
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
			return true;
		}
	}
	
	func textFieldDidBeginEditing(textField: UITextField) {
		typing = true
	}
	
	func textFieldDidEndEditing(textField: UITextField) {
		typing = false
		update()
		sharedMessageManager.tuning()
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
	
	func peripheralsChanged() {}
	func batteryChanged() {}
	func debugChanged() {}
	
	func configChanged() {
		update();
	}
	
	func update() {
		if (typing) {
			return
		}
		
		throttle.value = sharedModel.throttleSp
		
		if (axis.selectedSegmentIndex == 0) {
			rate.value = sharedModel.rateSp.x
			rateP.text = String.localizedStringWithFormat("%.3f", sharedModel.rateConfig.x.p)
			rateI.text = String.localizedStringWithFormat("%.3f", sharedModel.rateConfig.x.i)
			rateD.text = String.localizedStringWithFormat("%.3f", sharedModel.rateConfig.x.d)
			angleP.text = String.localizedStringWithFormat("%.3f", sharedModel.angleConfig.x.p)
			angleI.text = String.localizedStringWithFormat("%.3f", sharedModel.angleConfig.x.i)
			angleD.text = String.localizedStringWithFormat("%.3f", sharedModel.angleConfig.x.d)
			tau.text = String.localizedStringWithFormat("%.3f", sharedModel.compConfig.x)
			rateLabel.text = "\(sharedModel.rateSp.x)°/s"
			angleLabel.text = "\(sharedModel.angleSp.x)°/s"
		} else if (axis.selectedSegmentIndex == 1) {
			rate.value = sharedModel.rateSp.y
			rateP.text = String.localizedStringWithFormat("%.3f", sharedModel.rateConfig.y.p)
			rateI.text = String.localizedStringWithFormat("%.3f", sharedModel.rateConfig.y.i)
			rateD.text = String.localizedStringWithFormat("%.3f", sharedModel.rateConfig.y.d)
			angleP.text = String.localizedStringWithFormat("%.3f", sharedModel.angleConfig.y.p)
			angleI.text = String.localizedStringWithFormat("%.3f", sharedModel.angleConfig.y.i)
			angleD.text = String.localizedStringWithFormat("%.3f", sharedModel.angleConfig.y.d)
			tau.text = String.localizedStringWithFormat("%.3f", sharedModel.compConfig.y)
			rateLabel.text = "\(sharedModel.rateSp.y)°/s"
			angleLabel.text = "\(sharedModel.angleSp.y)°/s"
		} else {
			rate.value = sharedModel.rateSp.x
			rateP.text = String.localizedStringWithFormat("%.3f", sharedModel.rateConfig.z.p)
			rateI.text = String.localizedStringWithFormat("%.3f", sharedModel.rateConfig.z.i)
			rateD.text = String.localizedStringWithFormat("%.3f", sharedModel.rateConfig.z.d)
			angleP.text = String.localizedStringWithFormat("%.3f", sharedModel.angleConfig.z.p)
			angleI.text = String.localizedStringWithFormat("%.3f", sharedModel.angleConfig.z.i)
			angleD.text = String.localizedStringWithFormat("%.3f", sharedModel.angleConfig.z.i)
			tau.text = "N/A"
			rateLabel.text = "\(sharedModel.rateSp.z)°/s"
			angleLabel.text = "\(sharedModel.angleSp.z)°/s"
		}
		
		throttleLabel.text = "\(sharedModel.throttleSp)%"

	}
	
}

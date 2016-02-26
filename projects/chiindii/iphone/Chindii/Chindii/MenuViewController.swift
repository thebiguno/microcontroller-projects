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
	@IBOutlet var spLabel : UILabel!
	@IBOutlet var sp : UISlider!
	@IBOutlet var p : UITextField!
	@IBOutlet var i : UITextField!
	@IBOutlet var d : UITextField!
	@IBOutlet var beta : UITextField!
	@IBOutlet var battery : UILabel!
	@IBOutlet var debug : UILabel!
	@IBOutlet var status : UILabel!
	@IBOutlet var selectedPID : UILabel!
	@IBOutlet var selectedPeripheral : UILabel!
	
	var typing = false

	@IBAction func unwindToMenu(seque: UIStoryboardSegue) {
		update();
	}
	
	@IBAction func loadClicked(sender: AnyObject) {
		sharedMessageManager.revertTuning()
	}
	
	@IBAction func saveClicked(sender: AnyObject) {
		sharedMessageManager.saveTuning()
	}
	
	@IBAction func calibrateClicked(sender: AnyObject) {
		sharedMessageManager.calibrate()
	}
	
	@IBAction func armPressed(sender : UISwitch) {
		if (sender.on) {
			if (sharedModel.selectedPID < 3) {
				sharedMessageManager.armRate()
			} else if (sharedModel.selectedPID < 6) {
				sharedMessageManager.armAngle()
			} else {
				sharedMessageManager.armFlight()
			}
		} else {
			sharedMessageManager.disarm()
		}
	}
	
	@IBAction func throttleChanged(sender : UISlider) {
		sender.value = roundf(sender.value)
		
		sharedModel.throttleSp = sender.value * 10;
		throttleLabel.text = "\(sharedModel.throttleSp)%"
	}
	
	@IBAction func spChanged(sender : UISlider) {
		sender.value = roundf(sender.value)
		sharedModel.rateSp.x = 0;
		sharedModel.rateSp.y = 0;
		sharedModel.rateSp.z = 0;
		sharedModel.angleSp.x = 0;
		sharedModel.angleSp.y = 0;
		sharedModel.angleSp.z = 0;

		switch (sharedModel.selectedPID) {
		case 0:
			sharedModel.rateSp.x = sender.value
			spLabel.text = "\(sharedModel.rateSp.x)°/s"
			break;
		case 1:
			sharedModel.rateSp.y = sender.value
			spLabel.text = "\(sharedModel.rateSp.y)°/s"
			break;
		case 2:
			sharedModel.rateSp.z = sender.value
			spLabel.text = "\(sharedModel.rateSp.z)°/s"
			break;
		case 3:
			sharedModel.angleSp.x = sender.value
			spLabel.text = "\(sharedModel.angleSp.x)°"
			break;
		case 4:
			sharedModel.angleSp.y = sender.value
			spLabel.text = "\(sharedModel.angleSp.y)°"
			break;
		case 5:
			sharedModel.angleSp.z = sender.value
			spLabel.text = "\(sharedModel.angleSp.z)°"
			break;
		default:
			sharedModel.throttleSp = sender.value
			spLabel.text = "\(sharedModel.throttleSp) g"
		}
	}
	
	func textField(textField: UITextField, shouldChangeCharactersInRange range: NSRange, replacementString string: String) -> Bool {
		let updated = (textField.text! as NSString)
			.stringByReplacingCharactersInRange(range, withString: string);
		
		let numberFormatter = NSNumberFormatter()
		let number = numberFormatter.numberFromString(updated as String)
		if (updated == "" || updated == ".") {
			// not a number but still an valid input
			return true;
		} else if (number == nil) {
			return false;
		} else {
			let numberFloatValue = number!.floatValue

			if (textField == beta) { sharedModel.madgwickConfig = numberFloatValue }
			else {
				switch (sharedModel.selectedPID) {
				case 0:
					if (textField == p)	{ sharedModel.rateConfig.x.p = numberFloatValue }
					else if (textField == i) { sharedModel.rateConfig.x.i = numberFloatValue }
					else if (textField == d) { sharedModel.rateConfig.x.d = numberFloatValue }
					break;
				case 1:
					if (textField == p)	{ sharedModel.rateConfig.y.p = numberFloatValue }
					else if (textField == i) { sharedModel.rateConfig.y.i = numberFloatValue }
					else if (textField == d) { sharedModel.rateConfig.y.d = numberFloatValue }
					break;
				case 2:
					if (textField == p)	{ sharedModel.rateConfig.z.p = numberFloatValue }
					else if (textField == i) { sharedModel.rateConfig.z.i = numberFloatValue }
					else if (textField == d) { sharedModel.rateConfig.z.d = numberFloatValue }
					break;
				case 3:
					if (textField == p)	{ sharedModel.angleConfig.x.p = numberFloatValue }
					else if (textField == i) { sharedModel.angleConfig.x.i = numberFloatValue }
					else if (textField == d) { sharedModel.angleConfig.x.d = numberFloatValue }
					break;
				case 4:
					if (textField == p)	{ sharedModel.angleConfig.y.p = numberFloatValue }
					else if (textField == i) { sharedModel.angleConfig.y.i = numberFloatValue }
					else if (textField == d) { sharedModel.angleConfig.y.d = numberFloatValue }
					break;
				case 5:
					if (textField == p)	{ sharedModel.angleConfig.z.p = numberFloatValue }
					else if (textField == i) { sharedModel.angleConfig.z.i = numberFloatValue }
					else if (textField == d) { sharedModel.angleConfig.z.d = numberFloatValue }
					break;
				default:
					if (textField == p)	{ sharedModel.gforceConfig.p = numberFloatValue }
					else if (textField == i) { sharedModel.gforceConfig.i = numberFloatValue }
					else if (textField == d) { sharedModel.gforceConfig.d = numberFloatValue }
				}
				
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
	func batteryChanged() {
		battery.text = "\(sharedModel.battery)%"
	}
	func debugChanged() {
		debug.text = sharedModel.debug
	}
	func statusChanged() {
		status.text = sharedModel.status
	}
	
	func configChanged() {
		update();
	}
	
	func update() {
		if (typing) {
			return
		}
		
		selectedPeripheral.text = sharedMessageManager.connectedPeripheral?.name
		throttle.value = sharedModel.throttleSp
		beta.text = String.localizedStringWithFormat("%.3f", sharedModel.madgwickConfig)
		
		var value : PID
		var sp : Float
		var unit : String
		switch (sharedModel.selectedPID) {
		case 0:
			value = sharedModel.rateConfig.x
			sp = sharedModel.rateSp.x
			unit = "°/s"
			selectedPID.text = "X Roll Rate"
			break
		case 1:
			value = sharedModel.rateConfig.y
			sp = sharedModel.rateSp.y
			unit = "°/s"
			selectedPID.text = "Y Pitch Rate"
			break;
		case 2:
			value = sharedModel.rateConfig.z
			sp = sharedModel.rateSp.z
			unit = "°/s"
			selectedPID.text = "Z Yaw Rate"
			break
		case 3:
			value = sharedModel.angleConfig.x
			sp = sharedModel.angleSp.x
			unit = "°"
			selectedPID.text = "X Roll Angle"
			break
		case 4:
			value = sharedModel.angleConfig.y
			sp = sharedModel.angleSp.y
			unit = "°"
			selectedPID.text = "Y Pitch Angle"
			break
		case 5:
			value = sharedModel.angleConfig.z
			sp = sharedModel.angleSp.z
			unit = "°"
			selectedPID.text = "Z Yaw Angle"
			break
		default:
			value = sharedModel.gforceConfig
			sp = sharedModel.throttleSp
			unit = " g"
			selectedPID.text = "G-Force"
		}
		
		p.text = String.localizedStringWithFormat("%.3f", value.p)
		i.text = String.localizedStringWithFormat("%.3f", value.i)
		d.text = String.localizedStringWithFormat("%.3f", value.d)
		spLabel.text = "\(sp)\(unit)"

		throttleLabel.text = "\(sharedModel.throttleSp)%"

	}
	
}

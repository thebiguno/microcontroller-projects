//
//  PortraitViewController.swift
//  Chindii
//
//  Created by Warren Janssens on 2015-08-19.
//  Copyright (c) 2015 Warren Janssens. All rights reserved.
//

import UIKit

class MenuViewController: UITableViewController, UITextFieldDelegate {
	
	@IBOutlet var axis : UISegmentedControl!
	@IBOutlet var rate : UITextField!
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
		print("Load Clicked")
	}
	
	@IBAction func saveClicked(sender: AnyObject) {
		print("Save Clicked")
	}
	
	@IBAction func axisChanged(sender: UISegmentedControl) {
		print("Axis Changed")
		print(sender.selectedSegmentIndex)
	}
	
	func textField(textField: UITextField, shouldChangeCharactersInRange range: NSRange, replacementString string: String) -> Bool {
		print("textfield")
		if (textField == rate) {
			print("rate")
		} else if (textField == rateP) {
			print("rateP")
		}
		return true
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

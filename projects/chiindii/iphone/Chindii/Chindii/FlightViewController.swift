//
//  LandscapeViewController.swift
//  Chindii
//
//  Created by Warren Janssens on 2015-08-19.
//  Copyright (c) 2015 Warren Janssens. All rights reserved.
//

import UIKit
import CoreBluetooth

class FlightViewController: UIViewController {

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
	
	func serialHandlerDidReceiveMessage(message: String) {
	}
	
	func serialHandlerDidDisconnect(peripheral: CBPeripheral, error: NSError?) {
	}
	
	func serialHandlerDidChangeState(newState: CBCentralManagerState) {
	}
	
}

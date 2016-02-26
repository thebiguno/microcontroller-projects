//
//  ConfigModel.swift
//  Chindii
//
//  Created by Warren Janssens on 2016-01-24.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import Foundation
import CoreBluetooth

protocol ModelDelegate : NSObjectProtocol {
	func debugChanged()
	func statusChanged()
	func batteryChanged()
	func configChanged()
	func peripheralsChanged()
}

struct PID {
	var p : Float = 0.0
	var i : Float = 0.0
	var d : Float = 0.0
}

struct RateConfig {
	var x = PID()
	var y = PID()
	var z = PID()
}

struct AngleConfig {
	var x = PID()
	var y = PID()
	var z = PID()
}

struct Vector {
	var x : Float = 0.0
	var y : Float = 0.0
	var z : Float = 0.0
}

let sharedModel = Model();

class Model {
	var delegate : ModelDelegate?
	
	var peripherals = [CBPeripheral]() {
		didSet {
			delegate?.peripheralsChanged()
		}
	}

	// flight
	var angleSp = Vector()
	var throttleSp : Float = 0.0 // this is either a throttle percentage or a g-force setpoint, depending on the mode
	var battery : UInt8 = 0 {
		didSet {
			delegate?.batteryChanged()
		}
	}
	var debug = "" {
		didSet {
			delegate?.debugChanged()
		}
	}
	var status = "" {
		didSet {
			delegate?.statusChanged()
		}
	}

	// config
	var selectedPID = 0
	var rateSp = Vector()
	var rateConfig = RateConfig() {
		didSet {
			delegate?.configChanged()
		}
	}
	var angleConfig = AngleConfig() {
		didSet {
			delegate?.configChanged()
		}
	}
	var gforceConfig = PID() {
		didSet {
			delegate?.configChanged()
		}
	}
	var madgwickConfig : Float = 0.0 {
		didSet {
			delegate?.configChanged()
		}
	}

	init() {
		
	}
}

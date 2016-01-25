//
//  ConfigModel.swift
//  Chindii
//
//  Created by Warren Janssens on 2016-01-24.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import Foundation

class ConfigModel {
	var peripheras = NSMutableDictionary()
	var peripherasNames = [String]()
	var peripheralName = ""
	var xRateP : Float = 0.0
	var xRateI : Float = 0.0
	var xRateD : Float = 0.0
	var yRateP : Float = 0.0
	var yRateI : Float = 0.0
	var yRateD : Float = 0.0
	var zRateP : Float = 0.0
	var zRateI : Float = 0.0
	var zRateD : Float = 0.0
	var xAngleP : Float = 0.0
	var xAngleI : Float = 0.0
	var xAngleD : Float = 0.0
	var yAngleP : Float = 0.0
	var yAngleI : Float = 0.0
	var yAngleD : Float = 0.0
	var xTau : Float = 0.0
	var yTau : Float = 0.0
}

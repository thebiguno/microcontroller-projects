//
//  FlightModel.swift
//  Chindii
//
//  Created by Warren Janssens on 2016-01-24.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import Foundation

class FlightModel {
	var armed : Bool = false
	var xAngle : Float = 0.0
	var yAngle : Float = 0.0
	var throttle : Float = 0.0
	
	var xRate : Float = 0.0
	var yRate : Float = 0.0
	var zRate : Float = 0.0
	
	var battery : UInt8 = 0
}
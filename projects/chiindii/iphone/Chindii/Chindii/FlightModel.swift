//
//  FlightModel.swift
//  Chindii
//
//  Created by Warren Janssens on 2016-01-24.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import Foundation

struct Vector {
	var x : Float = 0.0
	var y : Float = 0.0
	var z : Float = 0.0
}

let sharedFlightModel = FlightModel()

class FlightModel {
	var armed : Bool = false
	var angle = Vector()
	var rate = Vector()
	var throttle : Float = 0.0
	var battery : UInt8 = 0
	
	init() {
		
	}
}
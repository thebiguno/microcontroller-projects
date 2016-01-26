//
//  ConfigModel.swift
//  Chindii
//
//  Created by Warren Janssens on 2016-01-24.
//  Copyright © 2016 Warren Janssens. All rights reserved.
//

import Foundation

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
}
struct CompConfig {
	var x : Float = 0.0
	var y : Float = 0.0
}

class ConfigModel {
	var peripheras = NSMutableDictionary()
	var peripherasNames = [String]()
	var peripheralName = ""
	var rate = RateConfig()
	var angle = RateConfig()
	var comp = CompConfig()
}

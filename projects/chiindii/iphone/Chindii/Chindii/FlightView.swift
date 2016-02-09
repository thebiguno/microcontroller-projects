//
//  MyView.swift
//  Chindii
//
//  Created by Warren Janssens on 2015-09-03.
//  Copyright (c) 2015 Warren Janssens. All rights reserved.
//

import Foundation
import UIKit

class FlightView: UIView {

	// color palette:
	// white: 1.0, 1.0, 0.8
	// red:   0.9, 0.4, 0.3
	// blue:  0.5, 0.7, 0.8

	var radius = CGFloat(0.0);
	var pitch = 0	// a value between -6 and +6 representing radians between -pi/6 and +pi/6 (5 degree steps)
	var roll = 0	// a value between -6 and +6 representing radians between -pi/6 and +pi/6 (5 degree steps)
	
	override func drawRect(rect: CGRect) {
		let context = UIGraphicsGetCurrentContext()
		CGContextSetRGBStrokeColor(context, 0.5, 0.7, 0.8, 1.0)
		CGContextSetRGBFillColor(context, 0.0, 0.0, 0.0, 1.0)
		CGContextSetLineCap(context, CGLineCap.Square)
		CGContextSetLineWidth(context, 1.0);
		CGContextSetAllowsAntialiasing(context, true)
		CGContextSetShouldAntialias(context, true)
		
//		CGContextFillRect(context, rect)

		radius = rect.height / 2.0 - 20
		
		CGContextSaveGState(context);
		
		// translate to the centre to draw the horizon
		CGContextTranslateCTM(context, center.x, center.y)

		// draw the artifical horizon main circle (same in both orientations)
		let r = CGRect(x: -radius, y: -radius, width: radius * 2, height: radius * 2)
		CGContextAddEllipseInRect(context, r)
		CGContextStrokePath(context)
		
		// draw the horizon lines in 5 degree increments
		CGContextSetLineDash(context, 0, [0,5], 2)

		CGContextMoveToPoint(context, -radius, 0)
		CGContextAddLineToPoint(context, radius, 0)

		CGContextMoveToPoint(context, 0, -radius)
		CGContextAddLineToPoint(context, 0, radius)

		CGContextStrokePath(context)
		
		CGContextSetLineDash(context, 0, [], 0)
	}
	
	override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
		for touch in touches {
			pitchRollTouch(touch)
		}

		setNeedsDisplay()
	}
	override func touchesMoved(touches: Set<UITouch>, withEvent event: UIEvent?) {
		for touch in touches {
			pitchRollTouch(touch)
		}
		
		setNeedsDisplay()
	}
	override func touchesEnded(touches: Set<UITouch>, withEvent event: UIEvent?) {
		setNeedsDisplay()
		sharedModel.angleSp.x = 0;
		sharedModel.angleSp.y = 0;
	}
	
	func pitchRollTouch(touch: UITouch) {
		let point = touch.locationInView(self)
		
		var roll = ((point.x - center.x) / radius) * 0.10;
		var pitch = ((point.y - center.y) / radius) * 0.10;
		
		// limit to .1 radian (~ 5 degrees)
		if (roll > 0.1) { roll = 0.1 }
		if (pitch > 0.1) { pitch = 0.1 }
		if (roll < -0.1) { roll = -0.1 }
		if (pitch < -0.1) { pitch = -0.1 }
		
		// TODO verify x and y
		sharedModel.angleSp.x = Float(pitch)
		sharedModel.angleSp.y = Float(roll)
		
//		print(sharedModel.angleSp.x)
//		print(sharedModel.angleSp.y)
	}
}

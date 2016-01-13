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

	var buttons = [CGRect]()
	var pressed = [false, false]
	
	var g = 0		// a value between -6 and +6 representing g force values between -1.5 and +1.5 (.25 g steps)
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
		
		CGContextFillRect(context, rect)

		let radius = rect.height / 2.0 - 20
		
		// draw the throttle position
		for i in -6..<7 {
			if (i == g) {
				CGContextSetRGBStrokeColor(context, 1.0, 1.0, 0.8, 1.0)
			} else {
				CGContextSetRGBStrokeColor(context, 0.5, 0.7, 0.8, 1.0)
			}
			
			if (i <= 0) {
				CGContextMoveToPoint(context, 20, rect.height / 2 - CGFloat(i*20))
				CGContextAddLineToPoint (context, 40, rect.height / 2 - CGFloat(i*20) + 20)
				CGContextAddLineToPoint (context, 60, rect.height / 2 - CGFloat(i*20))
				CGContextStrokePath(context)
			}
			
			if (i >= 0) {
				CGContextMoveToPoint(context, 20, rect.height / 2 - CGFloat(i*20))
				CGContextAddLineToPoint (context, 40, rect.height / 2 - CGFloat(i*20) - 20)
				CGContextAddLineToPoint (context, 60, rect.height / 2 - CGFloat(i*20))
				CGContextStrokePath(context)
			}
		}
		
		// draw the buttons
		buttons.removeAll(keepCapacity: true)
		for i in 0..<2 {
			let r = i == 0 ?
				CGRect(x: rect.width - 100, y: 20, width: 80, height: 80) :
				CGRect(x: rect.width - 100, y: rect.height - 100, width: 80, height: 80)
			
			buttons.append(r)
			
			CGContextAddEllipseInRect(context, r)
			if (pressed[i]) {
				CGContextSetRGBFillColor(context, 1.0, 1.0, 0.8, 1.0)
				CGContextFillPath(context)
			} else {
				CGContextSetRGBStrokeColor(context, 0.5, 0.7, 0.8, 1.0)
				CGContextStrokePath(context)
			}
		}
		
		// draw the take off / landing button
		CGContextSetRGBStrokeColor(context, 0.5, 0.7, 0.8, 1.0)
		CGContextMoveToPoint(context, rect.width - 40, 40)
		CGContextAddLineToPoint(context, rect.width - 60, 60)
		CGContextAddLineToPoint(context, rect.width - 80, 40)
		CGContextMoveToPoint(context, rect.width - 40, 60)
		CGContextAddLineToPoint(context, rect.width - 60, 80)
		CGContextAddLineToPoint(context, rect.width - 80, 60)
		CGContextStrokePath(context)
		
		// draw the battery indicator
		//CGContextSetLineWidth(context, 3.0);
		//CGContextMoveToPoint(context, rect.width - 60, 20)
		//CGContextAddCurveToPoint(context, rect.width - 7, 20, rect.width - 7, 100, rect.width - 60, 100)
		//CGContextStrokePath(context)
		
		
		//		CGContextSetBlendMode(context, kCGBlendModeNormal)
		
		
		// translate to the centre to draw the horizon
		CGContextTranslateCTM(context, center.x, center.y)

		// draw the artifical horizon main circle (same in both orientations)
		let r = CGRect(x: -radius, y: -radius, width: radius * 2, height: radius * 2)
		CGContextAddEllipseInRect(context, r)
		CGContextStrokePath(context)
		
		// draw the horizon lines in 5 degree increments
		CGContextSetLineDash(context, 0, [0,5], 2)
		// 0
		CGContextMoveToPoint(context, -140, 0)
		CGContextAddLineToPoint(context, +141, 0)
		// +/- 5
		CGContextMoveToPoint(context, -10, -20)
		CGContextAddLineToPoint(context, +11, -20)
		CGContextMoveToPoint(context, -10, +20)
		CGContextAddLineToPoint(context, +11, +20)
		// +/- 10
		CGContextMoveToPoint(context, -40, -40)
		CGContextAddLineToPoint(context, +41, -40)
		CGContextMoveToPoint(context, -40, +40)
		CGContextAddLineToPoint(context, +41, +40)
		// +/- 15
		CGContextMoveToPoint(context, -20, -60)
		CGContextAddLineToPoint(context, +21,  -60)
		CGContextMoveToPoint(context, -20, +60)
		CGContextAddLineToPoint(context, +21, +60)
		// +/- 20
		CGContextMoveToPoint(context, -40, -80)
		CGContextAddLineToPoint(context, +41, -80)
		CGContextMoveToPoint(context, -40, +80)
		CGContextAddLineToPoint(context, +41, +80)
		// +/- 35
		CGContextMoveToPoint(context, -30, -100)
		CGContextAddLineToPoint(context, +31, -100)
		CGContextMoveToPoint(context, -30, +100)
		CGContextAddLineToPoint(context, +31, +100)
		// +/- 40
		CGContextMoveToPoint(context, -60, -120)
		CGContextAddLineToPoint(context, +61, -120)
		CGContextMoveToPoint(context, -60, +120)
		CGContextAddLineToPoint(context, +61, +120)

		CGContextStrokePath(context)
		
		CGContextSetLineDash(context, 0, [], 0)
		
		// draw the artifical horizon
		// TODO, this has to move with the pitch / roll telemetry
		
		/*
		CGContextTranslateCTM(context, 0, CGFloat(-pitch*20))
		CGContextRotateCTM(context, CGFloat(M_PI / 6 / 6) * CGFloat(-roll));
		
		CGContextSetRGBStrokeColor(context, 1.0, 1.0, 0.8, 1.0)
		CGContextAddEllipseInRect(context, CGRect(x: -10, y: -10, width: 20, height: 20))
		CGContextMoveToPoint(context, -60, 0);
		CGContextAddLineToPoint(context, -10, 0)
		CGContextMoveToPoint(context, +10, 0)
		CGContextAddLineToPoint(context, +60, 0)
		CGContextStrokePath(context)
		*/
		
		CGContextSetRGBStrokeColor(context, 1.0, 1.0, 0.8, 1.0)

		let arcradius = CGFloat(abs(pitch) / 2) + CGFloat((2 * radius) * (2 * radius)) / CGFloat(8 * abs(pitch))
//		CGContextMoveToPoint(context, -radius, 0)
//		CGContextAddArcToPoint(context, 0, CGFloat(pitch), radius, 0, arcradius)

		let adj = arcradius + CGFloat(pitch)
		let op = radius
		print(tan(op / adj) * 57.296)
		let path = UIBezierPath(arcCenter: CGPoint(x: CGFloat(0), y: adj + CGFloat(pitch)), radius: arcradius, startAngle: CGFloat(3*M_PI/2) - tan(op / adj), endAngle: CGFloat(3*M_PI/2) + tan(op / adj), clockwise: true)
//		let path = UIBezierPath(arcCenter: CGPoint(x: CGFloat(0), y: arcradius + CGFloat(pitch)), radius: arcradius, startAngle: 0, endAngle: CGFloat(M_PI), clockwise: false)
		path.stroke()
		
//		CGContextMoveToPoint(context, -radius, 0)
//		CGContextAddQuadCurveToPoint(context, -radius, CGFloat(pitch), 0, CGFloat(pitch))
//		CGContextAddQuadCurveToPoint(context, radius, CGFloat(pitch), radius, 0)
//		CGContextStrokePath(context)

//		CGContextMoveToPoint(context, 0, -radius)
//		CGContextAddQuadCurveToPoint(context, CGFloat(roll), CGFloat(pitch), 0, arcradius)
//		CGContextStrokePath(context)

	}
	
	override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
		for touch in touches {
			let point = (touch ).locationInView(self)
			
			for i in 0..<2 {
				let r = buttons[i]
				pressed[i] = r.contains(point)
			}
			gTouch(touch );
			pitchRollTouch(touch)
		}

		setNeedsDisplay()
	}
	override func touchesMoved(touches: Set<UITouch>, withEvent event: UIEvent?) {
		for touch in touches {
			gTouch(touch );
			pitchRollTouch(touch)
		}
		
		setNeedsDisplay()
	}
	override func touchesEnded(touches: Set<UITouch>, withEvent event: UIEvent?) {
		for i in 0..<2 {
			pressed[i] = false
		}
		setNeedsDisplay()
	}
	
	func gTouch(touch: UITouch) {
		let point = touch.locationInView(self)

		for i in -6..<7 {
			let by = bounds.height / 2 - CGFloat(i*20) - (i >= 0 ? 20 : 0)
			let h = CGFloat(i == 0 ? 40 : 20)
			
			let rect = CGRect(x: 20, y: by, width: 40, height: h);
			if (rect.contains(point)) {
				g = i;
			}
		}
	}
	
	func pitchRollTouch(touch: UITouch) {
		let point = touch.locationInView(self)
		
		roll = Int(point.x - center.x)
		pitch = Int(point.y - center.y)
	}
}

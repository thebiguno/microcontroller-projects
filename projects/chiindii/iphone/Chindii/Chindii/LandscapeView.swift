//
//  MyView.swift
//  Chindii
//
//  Created by Warren Janssens on 2015-09-03.
//  Copyright (c) 2015 Warren Janssens. All rights reserved.
//

import Foundation
import UIKit

class LandscapeView: UIView {

	// color palette:
	// white: 1.0, 1.0, 0.8
	// red:   0.9, 0.4, 0.3
	// blue:  0.5, 0.7, 0.8
	
	var landscape = false
	
	var buttons = [CGRect]()
	var pressed = [false, false]
	
	var positions = [CGRect]()
	var position = 0
	
	override func drawRect(rect: CGRect) {
		landscape = rect.width > rect.height;
		
		let context = UIGraphicsGetCurrentContext()
		CGContextSetRGBStrokeColor(context, 0.5, 0.7, 0.8, 1.0)
		CGContextSetRGBFillColor(context, 0.0, 0.0, 0.0, 1.0)
		CGContextSetLineCap(context, kCGLineCapSquare)
		CGContextSetLineWidth(context, 1.0);
		CGContextSetAllowsAntialiasing(context, true)
		CGContextSetShouldAntialias(context, true)
		
		CGContextFillRect(context, rect)

		let width = rect.width
		let height = rect.height
		let radius = ((landscape ? rect.height : rect.width) / 2.0) - 20
		let centerX = width / 2
		let centerY = height / 2
		
		//		CGContextSetBlendMode(context, kCGBlendModeNormal)
		
		
		buttons.removeAll(keepCapacity: true)

		// draw the artifical horizon main circle (same in both orientations)
		let r = CGRect(x: centerX - radius, y: centerY - radius, width: radius * 2, height: radius * 2)
		CGContextAddEllipseInRect(context, r)
		CGContextStrokePath(context)
		
		// draw the artifical horizon
		// TODO, this has to move with the pitch / roll telemetry
		CGContextAddEllipseInRect(context, CGRect(x: centerX - 10, y: centerY - 10, width: 20, height: 20))
		CGContextMoveToPoint(context, centerX - 120, centerY);
		CGContextAddLineToPoint(context, centerX - 20, centerY)
		CGContextMoveToPoint(context, centerX + 20, centerY)
		CGContextAddLineToPoint(context, centerX + 120, centerY)
		CGContextStrokePath(context)
		
		CGContextSetLineDash(context, 0, [0,10], 2)
		CGContextMoveToPoint(context, centerX - 20, centerY - 20)
		CGContextAddLineToPoint(context, centerX + 21, centerY - 20)
		CGContextMoveToPoint(context, centerX - 20, centerY + 20)
		CGContextAddLineToPoint(context, centerX + 21, centerY + 20)
		CGContextMoveToPoint(context, centerX - 40, centerY - 40)
		CGContextAddLineToPoint(context, centerX + 41, centerY - 40)
		CGContextMoveToPoint(context, centerX - 40, centerY + 40)
		CGContextAddLineToPoint(context, centerX + 41, centerY + 40)
		CGContextMoveToPoint(context, centerX - 60, centerY - 60)
		CGContextAddLineToPoint(context, centerX + 61, centerY - 60)
		CGContextMoveToPoint(context, centerX - 60, centerY + 60)
		CGContextAddLineToPoint(context, centerX + 61, centerY + 60)
		CGContextMoveToPoint(context, centerX - 80, centerY - 80)
		CGContextAddLineToPoint(context, centerX + 81, centerY - 80)
		CGContextMoveToPoint(context, centerX - 80, centerY + 80)
		CGContextAddLineToPoint(context, centerX + 81, centerY + 80)
		CGContextStrokePath(context)
		
		CGContextSetLineDash(context, 0, [], 0)
		
		let points = [CGPoint]();
		if (landscape) {
			
			// draw the throttle position
			for i in 1..<15 {
				if (i == position) {
					CGContextSetRGBStrokeColor(context, 1.0, 1.0, 0.8, 1.0)
				} else {
					CGContextSetRGBStrokeColor(context, 0.5, 0.7, 0.8, 1.0)
				}
				CGContextMoveToPoint(context, 20, rect.height - CGFloat(i*20))
				CGContextAddLineToPoint (context, 40, rect.height - CGFloat(i*20) - 20)
				CGContextAddLineToPoint (context, 60, rect.height - CGFloat(i*20))
				CGContextStrokePath(context)
			}
			
			// draw the buttons
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
			
			// draw the menu button
			CGContextSetRGBStrokeColor(context, 0.5, 0.7, 0.8, 1.0)
			CGContextMoveToPoint(context, rect.width - 40, rect.height - 40)
			CGContextAddLineToPoint(context, rect.width - 80, rect.height - 40)
			CGContextMoveToPoint(context, rect.width - 40, rect.height - 60)
			CGContextAddLineToPoint(context, rect.width - 80, rect.height - 60)
			CGContextMoveToPoint(context, rect.width - 40, rect.height - 80)
			CGContextAddLineToPoint(context, rect.width - 80, rect.height - 80)
			CGContextStrokePath(context)
			
			// draw the battery indicator
			//CGContextSetLineWidth(context, 3.0);
			//CGContextMoveToPoint(context, rect.width - 60, 20)
			//CGContextAddCurveToPoint(context, rect.width - 7, 20, rect.width - 7, 100, rect.width - 60, 100)
			//CGContextStrokePath(context)
			
		}
		else {

			// draw the throttle position
			for i in 0..<16 {
				if (i == position) {
					CGContextSetRGBStrokeColor(context, 1.0, 1.0, 0.8, 1.0)
				} else {
					CGContextSetRGBStrokeColor(context, 0.5, 0.7, 0.8, 1.0)
				}
				CGContextMoveToPoint(context, CGFloat(i*20), rect.height - 60)
				CGContextAddLineToPoint (context, CGFloat(i*20) + 20, rect.height - 40)
				CGContextAddLineToPoint (context, CGFloat(i*20), rect.height - 20)
				CGContextStrokePath(context)
			}
			
			// draw the buttons
			for i in 0..<2 {
				let r = i == 0 ?
					CGRect(x: 20, y: 20, width: 80, height: 80) :
					CGRect(x: rect.width - 100, y: 20, width: 80, height: 80)
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
			CGContextMoveToPoint(context, 40, 40)
			CGContextAddLineToPoint(context, 60, 60)
			CGContextAddLineToPoint(context, 80, 40)
			CGContextMoveToPoint(context, 40, 60)
			CGContextAddLineToPoint(context, 60, 80)
			CGContextAddLineToPoint(context, 80, 60)
			CGContextStrokePath(context)

			// draw the menu button
			CGContextSetRGBStrokeColor(context, 0.5, 0.7, 0.8, 1.0)
			CGContextMoveToPoint(context, rect.width - 40, 40)
			CGContextAddLineToPoint(context, rect.width - 80, 40)
			CGContextMoveToPoint(context, rect.width - 40, 60)
			CGContextAddLineToPoint(context, rect.width - 80, 60)
			CGContextMoveToPoint(context, rect.width - 40, 80)
			CGContextAddLineToPoint(context, rect.width - 80, 80)
			CGContextStrokePath(context)
		}
	}
	
	override func touchesBegan(touches: Set<NSObject>, withEvent event: UIEvent) {
		for i in 0..<2 {
			let touch = touches.first as! UITouch
			let point = touch.locationInView(self)
			let r = buttons[i]
			pressed[i] = r.contains(point)
		}
		setNeedsDisplay()
	}
	override func touchesEnded(touches: Set<NSObject>, withEvent event: UIEvent) {
		for i in 0..<2 {
			pressed[i] = false
		}
		setNeedsDisplay()
	}
}

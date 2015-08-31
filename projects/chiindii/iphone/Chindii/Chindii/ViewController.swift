//
//  ViewController.swift
//  Chindii
//
//  Created by Warren Janssens on 2015-08-19.
//  Copyright (c) 2015 Warren Janssens. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

	@IBOutlet weak var imageView: UIImageView!
	
    override func viewDidLoad() {
        super.viewDidLoad()
		
		UIGraphicsBeginImageContext(view.frame.size)
        let context = UIGraphicsGetCurrentContext()
		CGContextSetAllowsAntialiasing(context, true)
		CGContextSetShouldAntialias(context, true)
        imageView.image?.drawInRect(CGRect(x: 0, y: 0, width: view.frame.size.width, height: view.frame.size.height))
		
		let width = view.frame.size.width;
		let height = view.frame.size.height;
		let radius = (view.bounds.size.height / 2.0) - 20
		let centerX = width / 2
		let centerY = height / 2
		
		let r = CGRect(x: centerX - radius, y: centerY - radius, width: radius * 2, height: radius * 2)
		
        CGContextMoveToPoint(context, centerX, centerY)
		CGContextAddEllipseInRect(context, r)
        CGContextSetLineCap(context, kCGLineCapRound)
        CGContextSetLineWidth(context, 1.0);
        CGContextSetRGBStrokeColor(context, 0.5, 0.65, 0.65, 1.0)
        CGContextSetBlendMode(context, kCGBlendModeNormal)
		
        CGContextStrokePath(context)
		
        imageView.image = UIGraphicsGetImageFromCurrentImageContext()
        
        UIGraphicsEndImageContext()
        
        
        // Do any additional setup after loading the view, typically from a nib.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
}

package ca.digitalcave.stubby;

import processing.core.PApplet;
import processing.serial.Serial;

public class Stubby {

	private final PApplet parent;
	private final Serial serial;
	
	public Stubby(PApplet parent) {
		this.parent = parent;
		
		for (String port : Serial.list()){
			if (port.toLowerCase().contains("stubby")){
				serial = new Serial(parent, port, 38400);
				return;
			}
		}
		
		throw new IllegalArgumentException("No serial port containing text 'stubby' could be found.  Please connect Stubby using a port which name contains text 'stubby', or specify the proper port using the Stubby(PApplet, String) constructor.");
	}
	
	public Stubby(PApplet parent, String port) {
		this.parent = parent;
		this.serial = new Serial(parent, port, 38400);
	}
	
	/**
	 * Move the specified number of steps in the direction 
	 * indicated (direction is in radians).  0 is forward; 
	 * Math.PI (or -Math.PI) is backwards; Math.PI / 2 is right; 
	 * -Math.PI / 2 is left.
	 * 
	 * A step is considered one complete cycle through all foot 
	 * positions.
	 *  
	 * @param steps
	 * @param direction
	 */
	public void move(int steps, double direction){
		if (steps < 0) return;
		serial.write("foo");
	}
	
	/**
	 * Convenience method to walk forward.
	 * @param steps
	 */
	public void moveForward(int steps){
		move(steps, 0);
	}

	/**
	 * Convenience method to walk backward.
	 * @param steps
	 */
	public void moveBackward(int steps){
		move(steps, Math.PI);
	}
	
	/**
	 * Convenience method to walk right.
	 * @param steps
	 */
	public void moveRight(int steps){
		move(steps, Math.PI / 2);
	}

	/**
	 * Convenience method to walk right.
	 * @param steps
	 */
	public void moveLeft(int steps){
		move(steps, Math.PI / -2);
	}
	
	/**
	 * Rotates the specified number of steps.  Positive values rotate
	 * clockwise, negative rotate counter clockwise.
	 * @param steps
	 */
	public void rotate(int steps){
		
	}
	
	/**
	 * Reads the distance sensor (value in mm)
	 * @return
	 */
	public long readDistanceSensor(){
		return 0;
	}
	
	/**
	 * Sets the distance servo angle (in radians).  0 is straight forward; negative 
	 * values are looking down, positive values are looking up.
	 * @param angle
	 */
	public void setDistanceAngle(double angle){
		
	}
	
	/**
	 * Reads the specified optical sensor (return value will be a byte between 0 and 255)
	 * @param sensor The sensor index; 0 is furthest left, and increments going right.
	 * @return
	 */
	public int readOpticalSensor(int sensor){
		return 0;
	}
	
	public void dispose(){
		serial.stop();
	}
}

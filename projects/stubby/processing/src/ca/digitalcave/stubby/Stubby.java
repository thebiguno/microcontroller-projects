package ca.digitalcave.stubby;

import processing.core.PApplet;
import processing.serial.Serial;

public class Stubby {
	//private final PApplet parent;
	private final Protocol protocol;
	
	public Stubby(PApplet parent) {
		//this.parent = parent;
		
		for (String port : Serial.list()){
			if (port.toLowerCase().contains("stubby")){
				final StubbySerial serial = new StubbySerial(parent, port, 38400);
				protocol = new Protocol(serial);
				serial.setProtocol(protocol);
				return;
			}
		}
		
		throw new IllegalArgumentException("No serial port containing text 'stubby' could be found.  Please connect Stubby using a port which name contains text 'stubby', or specify the proper port using the Stubby(PApplet, String) constructor.");
	}
	
	public Stubby(PApplet parent, String port) {
		final StubbySerial serial = new StubbySerial(parent, port, 38400);
		protocol = new Protocol(serial);
		serial.setProtocol(protocol);
	}
	
	/**
	 * Move the specified number of mm in the indicated direction at the given speed
	 * 
	 * Distance is measured in mm
	 *  
	 * @param distance Measured in mm
	 * @param speed Value from 0 to 255.  255 is fastest.
	 * @params direction Value in radians; 0 is forward; Math.PI (or -Math.PI) is backwards; Math.PI / 2 is right; -Math.PI / 2 is left.
	 */
	public void move(int distance, int speed, float direction){
		if (distance == 0) return;
		if (distance < 0) direction = direction + (float) Math.PI;	//Support going backwards
		int[] data = new int[7];
		data[0] = Protocol.radianToByte(direction);
		data[1] = speed & 0xFF;
		data[2] = (distance >> 8) & 0xFF;
		data[3] = distance & 0xFF;
		protocol.sendMessage(new Message(Protocol.REQUEST_MOVE, data));
	}
	
	public void move(int distance, float direction){
		move(distance, 0xFF, direction);
	}
	
	/**
	 * Convenience method to walk forward.
	 * @param steps
	 */
	public void moveForward(int distance){
		move(distance, 0);
	}

	/**
	 * Convenience method to walk backward.
	 * @param steps
	 */
	public void moveBackward(int distance){
		move(distance, (float) Math.PI);
	}
	
	/**
	 * Convenience method to walk right.
	 * @param steps
	 */
	public void moveRight(int steps){
		move(steps, (float) Math.PI / 2);
	}

	/**
	 * Convenience method to walk right.
	 * @param steps
	 */
	public void moveLeft(int steps){
		move(steps, (float) Math.PI / -2);
	}
	
	/**
	 * Rotates to the specified angle.  Positive values rotate
	 * clockwise, negative rotate counter clockwise.
	 * @param angle In radians, with 0 being the direction the robot is currently facing.
	 */
	public void turn(float angle){
		
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
		protocol.getSerial().stop();
	}
}

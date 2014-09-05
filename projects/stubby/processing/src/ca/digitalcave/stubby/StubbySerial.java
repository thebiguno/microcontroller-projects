package ca.digitalcave.stubby;

import jssc.SerialPortEvent;
import processing.core.PApplet;
import processing.serial.Serial;

public class StubbySerial extends Serial {
	private Protocol protocol;
	
	public StubbySerial(PApplet parent, String iname, int irate) {
		super(parent, iname, irate);
	}
	
	synchronized public void serialEvent(SerialPortEvent serialEvent) {
		if (serialEvent.getEventType() == SerialPortEvent.RXCHAR) {
			if (protocol != null) protocol.receiveByte();
			else System.out.println("Protocol is null");
		}
	}
	
	public void setProtocol(Protocol protocol){
		this.protocol = protocol;
	}
}

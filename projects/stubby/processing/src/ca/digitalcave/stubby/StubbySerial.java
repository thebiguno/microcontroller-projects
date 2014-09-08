package ca.digitalcave.stubby;

import jssc.SerialPortEvent;
import jssc.SerialPortException;
import processing.core.PApplet;
import processing.serial.Serial;

public class StubbySerial extends Serial {
	private Protocol protocol;

	public StubbySerial(PApplet parent, String iname, int irate) {
		super(parent, iname, irate);
	}

	synchronized public void serialEvent(SerialPortEvent serialEvent) {
		try {
			if (serialEvent.getEventType() == SerialPortEvent.RXCHAR) {
				if (protocol != null)
					protocol.receiveByte(port.readBytes(1)[0] & 0xFF);
			}
		} catch (SerialPortException e) {
			System.err.println(e.getMessage());
		}
	}

	public void setProtocol(Protocol protocol){
		this.protocol = protocol;
	}
}

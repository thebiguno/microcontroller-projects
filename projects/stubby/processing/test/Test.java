import ca.digitalcave.stubby.Stubby;
import processing.core.PApplet;
import processing.serial.Serial;

public class Test extends PApplet {
	public static void main(String args[]) {
		PApplet.main(new String[] { "Test" });
	}
	
	public void setup(){
		Stubby stubby = new Stubby(this, "/dev/tty.usbserial-FTE0U36U");
	}

	public void serialEvent(Serial serial){
		System.out.println(Integer.toHexString(serial.read()));
	}
	
	public void draw() {
		
	}
}

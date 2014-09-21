import processing.core.PApplet;
import processing.serial.Serial;
import ca.digitalcave.stubby.Stubby;

public class Test extends PApplet {
	public static void main(String args[]) {
		PApplet.main(new String[] { "Test" });
	}
	
	public void setup(){
		Stubby stubby = new Stubby(this, "/dev/rfcomm0");
		stubby.turnOn();
//		stubby.move(5 * Math.PI / 6, 500);
//		stubby.move(11 * Math.PI / 6, 500);
//		stubby.moveForward(1000);
//		stubby.moveBackward(500);
		stubby.moveForward(300);
		stubby.turnAroundClockwise();
		stubby.moveForward(300);
		stubby.turnAround();
		stubby.turnOff();
	}

	public void serialEvent(Serial serial){
		System.out.println(Integer.toHexString(serial.read()));
	}
	
	public void draw() {
		System.exit(0);
	}
}

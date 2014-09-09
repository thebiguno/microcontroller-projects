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
		stubby.moveForward(200);
		stubby.moveLeft(200);
		stubby.moveBackward(200);
		stubby.moveRight(200);
		stubby.turnOff();
	}

	public void serialEvent(Serial serial){
		System.out.println(Integer.toHexString(serial.read()));
	}
	
	public void draw() {
		
	}
}

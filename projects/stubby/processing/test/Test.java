import processing.core.PApplet;
import processing.serial.Serial;
import ca.digitalcave.stubby.Stubby;

public class Test extends PApplet {
	public static void main(String args[]) {
		PApplet.main(new String[] { "Test" });
	}
	
	public void setup(){
		Stubby stubby = new Stubby(this, "/dev/ttyUSB0");
		stubby.enableDebug();
		stubby.turnOn();
		stubby.moveForward(1000);
		stubby.moveBackward(1000);
		//stubby.turnAroundClockwise();
		//stubby.moveForward(1000);
//		stubby.moveRight(1000);
//		stubby.moveLeft(1000);
//		stubby.turnLeft();
//		stubby.turnRight();
		//stubby.moveForward(1000);
//		stubby.moveForward(300);
//		stubby.turnAroundClockwise();
//		stubby.moveForward(300);
//		stubby.turnAround();
//		for (int i = 0; i < 100; i++){
//			stubby.requestHeading();
//			try {Thread.sleep(100);} catch (InterruptedException e) {}
//		}
//		try {Thread.sleep(10000);} catch (InterruptedException e) {}
		stubby.turnOff();
	}

	public void serialEvent(Serial serial){
		System.out.println(Integer.toHexString(serial.read()));
	}
	
	public void draw() {
		System.exit(0);
	}
}

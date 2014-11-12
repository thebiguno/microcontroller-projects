import java.io.IOException;

import com.codeminders.hidapi.ClassPathLibraryLoader;
import com.codeminders.hidapi.HIDDevice;
import com.codeminders.hidapi.HIDDeviceNotFoundException;
import com.codeminders.hidapi.HIDDeviceInfo;
import com.codeminders.hidapi.HIDManager;

public class Main {

	private static byte getRandom(){
		byte r = (byte) (Math.random() * 9);
		if (r == 0) r = (byte) ' ';
		else if (r == 8) r = (byte) 0xFF;
		
		return r;
	}

	public static void main(String[] args) throws Exception {
		ClassPathLibraryLoader.loadNativeHIDLibrary();

		HIDDevice device = HIDManager.getInstance().openById(0x4200, 0xFF00, null);

		if (args.length == 1){
			final byte buf[] = new byte[2];
			char type = args[0].charAt(0);
			int value = Integer.parseInt(args[0].replaceAll("[^0-9]", "")) & 0xFF;
			if (type == 'C') buf[0] = 0x02;
			else if (type == 'B') buf[0] = 0x03;
			else throw new Exception("Invalid command");
			buf[1] = (byte) value;
			device.write(buf);
			System.out.println(bytesToHex(buf));
		
			System.exit(0);
		}

		final byte buf[] = (" CXXXXMXXRXWXUXDX/home 99% 36°57°").getBytes();
		buf[0] = (byte) 0x01;	//Message type
		buf[29] = (byte) 0xDF;	//Degrees
		buf[32] = (byte) 0xDF;	//Degrees

		while (true) {
			buf[2] = getRandom();
			buf[3] = getRandom();
			buf[4] = getRandom();
			buf[5] = getRandom();
			buf[7] = getRandom();
			buf[8] = getRandom();
			buf[10] = getRandom();
			buf[12] = getRandom();
			buf[14] = getRandom();
			buf[16] = getRandom();
			buf[23] = (byte) (Math.random() * 10 + 0x30);
			buf[24] = (byte) (Math.random() * 10 + 0x30);
			buf[27] = (byte) (Math.random() * 10 + 0x30);
			buf[28] = (byte) (Math.random() * 10 + 0x30);
			buf[30] = (byte) (Math.random() * 10 + 0x30);
			buf[31] = (byte) (Math.random() * 10 + 0x30);
			device.write(buf);
			System.out.println(bytesToHex(buf));
			Thread.sleep(1000);
		}
	}

	final protected static char[] hexArray = "0123456789ABCDEF".toCharArray();
	public static String bytesToHex(byte[] bytes) {
		final char[] hexChars = new char[bytes.length * 3];
		for ( int j = 0; j < bytes.length; j++ ) {
			int v = bytes[j] & 0xFF;
			hexChars[j * 3] = hexArray[v >>> 4];
			hexChars[j * 3 + 1] = hexArray[v & 0x0F];
			hexChars[j * 3 + 2] = ' ';
		}
		return new String(hexChars);
	}
}

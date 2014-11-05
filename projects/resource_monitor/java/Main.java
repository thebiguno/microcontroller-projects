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

		HIDDevice device = HIDManager.getInstance().openById(0x16c0, 0x0480, null);

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

		final byte buf[] = (" CXXXMXXdd WXXUXX#GXXMXXdd RXXDXX").getBytes();
		buf[0] = (byte) 0x01;
		buf[9] = (byte) 0xDF;
		buf[25] = (byte) 0xDF;

		while (true) {
			buf[2] = getRandom();
			buf[3] = getRandom();
			buf[4] = getRandom();
			buf[6] = getRandom();
			buf[7] = getRandom();
			buf[8] = (byte) (Math.random() * 10 + 0x30);
			buf[9] = (byte) (Math.random() * 10 + 0x30);
			buf[12] = getRandom();
			buf[13] = getRandom();
			buf[15] = getRandom();
			buf[16] = getRandom();
			buf[17] = (byte) (Math.random() > 0.9 ? 0xEF : ' ');
			buf[19] = getRandom();
			buf[20] = getRandom();
			buf[22] = getRandom();
			buf[23] = getRandom();
			buf[24] = (byte) (Math.random() * 10 + 0x30);
			buf[25] = (byte) (Math.random() * 10 + 0x30);
			buf[28] = getRandom();
			buf[29] = getRandom();
			buf[31] = getRandom();
			buf[32] = getRandom();
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

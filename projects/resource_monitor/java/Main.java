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

	public static void main(String[] args) throws HIDDeviceNotFoundException, IOException, InterruptedException {
		ClassPathLibraryLoader.loadNativeHIDLibrary();

		HIDDevice device = HIDManager.getInstance().openById(0x16c0, 0x0480, null);

		final byte buf[] = ("CXXXMXXdd WXXUXX#GXXMXXdd RXXDXX").getBytes();

		buf[9] = (byte) 0xDF;
		buf[25] = (byte) 0xDF;

		while (true) {
			buf[1] = getRandom();
			buf[2] = getRandom();
			buf[3] = getRandom();
			buf[5] = getRandom();
			buf[6] = getRandom();
			buf[7] = (byte) (Math.random() * 10 + 0x30);
			buf[8] = (byte) (Math.random() * 10 + 0x30);
			buf[11] = getRandom();
			buf[12] = getRandom();
			buf[14] = getRandom();
			buf[15] = getRandom();
			buf[16] = (byte) (Math.random() > 0.9 ? 0xEF : ' ');
			buf[18] = getRandom();
			buf[19] = getRandom();
			buf[21] = getRandom();
			buf[22] = getRandom();
			buf[23] = (byte) (Math.random() * 10 + 0x30);
			buf[24] = (byte) (Math.random() * 10 + 0x30);
			buf[27] = getRandom();
			buf[28] = getRandom();
			buf[30] = getRandom();
			buf[31] = getRandom();
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

import java.io.IOException;

import com.codeminders.hidapi.ClassPathLibraryLoader;
import com.codeminders.hidapi.HIDDevice;
import com.codeminders.hidapi.HIDDeviceNotFoundException;
import com.codeminders.hidapi.HIDDeviceInfo;
import com.codeminders.hidapi.HIDManager;

public class Main {

	public static void main(String[] args) throws HIDDeviceNotFoundException, IOException {
		ClassPathLibraryLoader.loadNativeHIDLibrary();

		HIDDevice device = HIDManager.getInstance().openById(0x16c0, 0x0480, null);

		final byte buf[] = new byte[64];
		byte b0 = 0x00;

		while (true) {
			device.read(buf);
			System.out.println(new String(buf));
			b0 ^= 0xFF;	//Send a different byte each time
			buf[0] = b0;
			device.write(buf);
			System.out.println(bytesToHex(buf));
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

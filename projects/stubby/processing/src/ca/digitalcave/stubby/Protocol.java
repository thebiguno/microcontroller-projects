package ca.digitalcave.stubby;

import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

import processing.serial.Serial;


public class Protocol {
	private final Serial serial;
	
	public final ConcurrentHashMap<Integer, List<Message>> mailbox = new ConcurrentHashMap<Integer, List<Message>>();

	public final int START = 0x7e;
	public final int ESCAPE = 0x7d;

	private final int MAX_SIZE = 255;
	
	public static final int SEND_ACKNOWLEDGE = 0x01;
	public static final int SEND_COMPLETE = 0x02;

	public static final int REQUEST_POWER_ON	= 0x20;
	public static final int REQUEST_POWER_OFF = 0x21;

	public static final int REQUEST_MOVE = 0x22;
	public static final int REQUEST_TURN = 0x23;
	public static final int REQUEST_TRANSLATE = 0x24;
	public static final int REQUEST_ROTATE = 0x25;

	public static final int REQUEST_DISTANCE = 0x26;
	public static final int SEND_DISTANCE = 0x27;

	public static final int REQUEST_OPTICAL = 0x28;
	public static final int SEND_OPTICAL = 0x29;

	public static final int REQUEST_SET_LED = 0x2A;

	
	public Protocol(Serial serial) {
		this.serial = serial;
	}
	
	public Serial getSerial() {
		return serial;
	}
	
	protected void dispatch(int cmd, int[] data, int length){
		System.out.print("Received command: " + Integer.toHexString(cmd) + ", value: ");
		for (int i = 0; i < length; i++) {
			System.out.print(Integer.toHexString(data[i]) + ", ");
		}
		System.out.println();
		if (mailbox.get(cmd) == null) mailbox.put(cmd, Collections.synchronizedList(new LinkedList<Message>()));
		mailbox.get(cmd).add(new Message(cmd, data));
		if (mailbox.get(cmd).size() > 255){
			mailbox.get(cmd).remove(0);	//Limit the length of the array by removing the oldest entries.
		}
	}

	private int[] buf = new int[MAX_SIZE];
	private boolean err = false;
	private boolean esc = false;
	private int pos = 0;
	private int len = 0;
	private int cmd = 0;
	private int chk = 0x00;
	
	public void receiveByte(int b) {
		if (err && b == START) {
			// recover from error condition
			System.err.println("Recover from error condition");
			err = false;
			pos = 0;
		} else if (err) {
			return;
		}

		if (pos > 0 && b == START) {
			// unexpected start of frame
			System.err.println("Unexpected start of frame");
			err = true;
			return;
		}

		if (pos > 0 && b == ESCAPE) {
			// unescape next byte
			esc = true;
			return;
		}

		if (esc) {
			// unescape current byte
			b = 0x20 ^ b;
			esc = false;
		}

		if (pos > 1) {
			chk = (chk + b) & 0xFF;
		}

		switch (pos) {
		case 0:
			pos++;
			return;
		case 1:
			len = b;
			pos++;
			return;
		case 2:
			cmd = b;
			pos++;
			return;
		default:
			if (pos > MAX_SIZE) {
				// this probably can't happen
				System.err.println("Position > MAX_SIZE");
				return;
			}
			if (pos == (len + 2)) {
				if (chk == 0xff) {
					dispatch(cmd, buf, pos - 3);
				} else {
					err = true;
					System.err.println("Invalid checksum");
				}
				pos = 0;
				chk = 0;
			} else {
				buf[pos++ - 3] = b;
			}
		}
	}

	private void sendByte(int b, boolean escape) {
		if (escape && (b == START || b == ESCAPE)) {
			serial.write(ESCAPE);
			serial.write(b ^ 0x20);
		} else {
			serial.write(b);
		}
	}

	public void sendMessage(Message message) {
		sendByte(START, false);
		sendByte(message.getData().length + 1, true);
		sendByte(message.getCommand(), true);

		int checksum = message.getCommand() & 0xFF;

		for (int i = 0; i < message.getData().length; i++) {
			sendByte(message.getData()[i], true);
			checksum += message.getData()[i];
			checksum &= 0xFF;
		}

		checksum = (0xFF - checksum);

		sendByte(checksum, true);
	}

	public static float byteToRadian(int x) {
		float r = 0.024639942381096f * x;
		return (r > Math.PI) ? r - (2.0f * (float) Math.PI) : r; // convert quadrants III & IV into negative values
	}
	public static int radianToByte(float x) {
		float r = x < 0 ? 2.0f * (float) Math.PI + x : x; // convert quadrants III & IV into positive values
		return (int) (40.584510488433314f * r);
	}

	public static void floatToArr(float f, int[] b, int offset) {
		int bits = Float.floatToIntBits(f);
		for (int i = offset; i < offset + 4; i++) {
			b[i] = bits & 0xFF;
			bits = bits >> 8;
		}		
	}
	public static float arrToFloat(int[] b, int offset) {
		int bits = 0;
		for (int i = offset + 3; i >= offset; i--){
			bits = (bits << 8) + b[i];
		}
		return Float.intBitsToFloat(bits);
	}
}

# Sample program demonstrating USB Raw HID access using the pyhidapi library
# (a ctypes wrapper around hidapi).  Sends alternating arrays containing
# 0xFF and 0x00, and prints the byte values of any recieved data.
#
# Designed to interface with usb_rawhid sample on the atmega32u2
#############################################################################
import hid, traceback, time

dev = 0

MESSAGE_SEND_ACTUAL				= 0
MESSAGE_SEND_RAW				= 1
MESSAGE_REQUEST_SETPOINT		= 2
MESSAGE_SEND_SETPOINT			= 3


def read_actual():
	try:
		dev.nonblocking = 0
		b = '\x00'

		while True:
			arr = dev.read(6)
			voltage = (ord(arr[2]) << 8) + ord(arr[3]);
			current = (ord(arr[4]) << 8) + ord(arr[5]);
			print("Read channel " + str(ord(arr[1])) + ", voltage=" + str(voltage) + ", current=" + str(current) + "\n")
	except Exception, e:
		traceback.print_exc()

def calibrate():
	channel = int(raw_input("Channel (0-5): "))
	if channel < 0 or channel > 5:
		print("Channel must be between 0 and 5")
		return;
	
	buf = list("  ")

	raw_input("Set the voltage output to 0.00V (using an external multimeter).  Hit enter when stabilized.")
	buf[0] = MESSAGE_SEND_RAW
	buf[1] = channel
	dev.write(''.join(buf))
	arr = dev.read(6)
	raw_voltage_low = arr[2] << 8 + arr[3]
	
	raw_input("Disconnect all output load (so that the channel outputs 0.00A).  Hit enter when stabilized.")
	buf[0] = MESSAGE_SEND_RAW
	buf[1] = channel
	dev.write(''.join(buf))
	arr = dev.read(6)
	raw_current_low = arr[4] << 8 + arr[5]

	raw_input("Set the voltage output to 8.00V (using an external multimeter).  Hit enter when stabilized.")
	buf[0] = MESSAGE_SEND_RAW
	buf[1] = channel
	dev.write(''.join(buf))
	arr = dev.read(6)
	raw_voltage_high = arr[2] << 8 + arr[3]
	
	raw_input("Connect a load so that the channel outputs 1.00A.  Hit enter when stabilized.")
	buf[0] = MESSAGE_SEND_RAW
	buf[1] = channel
	dev.write(''.join(buf))
	arr = dev.read(6)
	raw_current_high = arr[4] << 8 + arr[5]

	print(raw_voltage_low)
	print(raw_current_low)
	print(raw_voltage_high)
	print(raw_current_high)

########## Main startup hooks here ##########
if (__name__=="__main__"):
	try:
		while (True):
			try:
				dev = hid.Device(vid=0x4200, pid=0xFF01)

				if (len(sys.argv) == 1):
					read_actual()
				elif (len(sys.argv) == 2 and sys.argv[1] == "-C"):
					calibrate()
				else:
					print("Usage: " + sys.argv[0] + " [-C | -B NUM]\n\n\t-C\tRun through the calibration routines")
					sys.exit(-1)
			except hid.HIDException:
				sleep(1)
			finally:
				if ("close" in dir(dev)):
					dev.close()
	except:
		raise

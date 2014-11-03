# Resource monitor - client-side daemon
#
# Uses psutil to monitor system resources, and updates the data over USB using raw HID packets
# to the hardware monitor.
#
########################################################

import psutil
import struct
import sys
from time import sleep
from collections import namedtuple

line1 = ['c','X','m','X',' ','D','r','X','w','X',' ','N','d','X','u','X']
line2 = ['/','X',' ','H','X',' ','D','X',' ','B','X',' ','S','X',' ',' ']

def main():
	while True:
		updateCpuUsage()
		updateMemoryUsage()
		updateNetworkUsage()
		updateDiskUsage()
		printData()
		sleep(1)

def printData():
	sys.stdout.write(''.join(line1) + "      " + ''.join(line2) + '\r')
	sys.stdout.flush()

def updateCpuUsage():
	line1[1] = str(int(psutil.cpu_percent() / 12))
	return

def updateMemoryUsage():
	line1[3] = str(int(psutil.virtual_memory().percent / 12))
	return

def updateNetworkUsage():
	current = psutil.net_io_counters()
	
	if not hasattr(updateNetworkUsage, 'last'):
		updateNetworkUsage.last = {"sent": current.bytes_sent, "recv": current.bytes_recv}

	sent = current.bytes_sent - updateNetworkUsage.last["sent"]
	recv = current.bytes_recv - updateNetworkUsage.last["recv"]

	updateNetworkUsage.last = {"sent": current.bytes_sent, "recv": current.bytes_recv}

	line1[13] = str(len(str(recv)))
	line1[15] = str(len(str(sent)))

	return

def updateDiskUsage():
	current = psutil.disk_io_counters()
	
	if not hasattr(updateDiskUsage, 'last'):
		updateDiskUsage.last = {"read": current.read_bytes, "write": current.write_bytes}

	read = current.read_bytes - updateDiskUsage.last["read"]
	write = current.write_bytes - updateDiskUsage.last["write"]

	updateDiskUsage.last = {"read": current.read_bytes, "write": current.write_bytes}

	line1[7] = str(len(str(read)))
	line1[9] = str(len(str(write)))
	
	line2[1] = str(int(psutil.disk_usage("/").percent / 12))
	line2[4] = str(int(psutil.disk_usage("/home").percent / 12))
	line2[7] = str(int(psutil.disk_usage("/mnt/data").percent / 12))
	line2[10] = str(int(psutil.disk_usage("/mnt/backup").percent / 12))
	line2[13] = str(int(psutil.swap_memory().percent / 12))

	return

########## Main startup hooks here ##########
if (__name__=="__main__"):
	main()

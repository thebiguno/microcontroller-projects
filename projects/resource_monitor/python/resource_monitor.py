# Resource monitor - client-side daemon
#
# Uses psutil to monitor system resources, and updates the data over USB using raw HID packets
# to the hardware monitor.
#
# All packets are sent within 64 bytes.  The first byte is the type of packet; the remaining
# 63 bytes are packet dependent.  All unused bytes in a packet are set to zero.
#
########################################################

import psutil
import struct
import sys
from time import sleep
from collections import namedtuple

line1 = ['c','9','9','%',' ','w','9','9','9','k',' ','u','9','9','9','k']
line2 = ['m','9','9','%',' ','r','9','9','9','k',' ','d','9','9','9','k']

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
	cpu = int(psutil.cpu_percent())
	line1[1] = str(cpu / 10)
	line1[2] = str(cpu % 10)
	return

def updateMemoryUsage():
	mem = int(psutil.virtual_memory().percent)
	line2[1] = str(mem / 10)
	line2[2] = str(mem % 10)
	return

def updateNetworkUsage():
	current = psutil.net_io_counters()
	
	if not hasattr(updateNetworkUsage, 'last'):
		updateNetworkUsage.last = {"sent": current.bytes_sent, "recv": current.bytes_recv}

	sent = formatBytes(current.bytes_sent - updateNetworkUsage.last["sent"]).rjust(4)
	recv = formatBytes(current.bytes_recv - updateNetworkUsage.last["recv"]).rjust(4)
	updateNetworkUsage.last = {"sent": current.bytes_sent, "recv": current.bytes_recv}

	for x in range(4):
		line1[x + 12] = sent[x]
		line2[x + 12] = recv[x]

	return

def updateDiskUsage():
	current = psutil.disk_io_counters()
	
	if not hasattr(updateDiskUsage, 'last'):
		updateDiskUsage.last = {"read": current.read_bytes, "write": current.write_bytes}

	read = formatBytes(current.read_bytes - updateDiskUsage.last["read"]).rjust(4)
	write = formatBytes(current.write_bytes - updateDiskUsage.last["write"]).rjust(4)
	updateDiskUsage.last = {"read": current.read_bytes, "write": current.write_bytes}

	for x in range(4):
		line1[x + 6] = write[x]
		line2[x + 6] = read[x]

	return

def updatePacket(packet):
	while len(packet) < 64:
		packet.append(0)
	print(packet)

def formatBytes(n):
	format="%(value)i%(symbol)s"
	symbols = ('B', 'K', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y')
	prefix = {}
	for i, s in enumerate(symbols[1:]):
		prefix[s] = 1 << (i+1)*10
	for symbol in reversed(symbols[1:]):
		if n >= prefix[symbol]:
			value = float(n) / prefix[symbol]
			return format % locals()
	return format % dict(symbol=symbols[0], value=n)

########## Main startup hooks here ##########
if (__name__=="__main__"):
	main()

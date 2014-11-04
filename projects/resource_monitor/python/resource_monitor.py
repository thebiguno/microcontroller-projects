# Resource monitor - client-side daemon
#
# Uses psutil to monitor system resources, and updates the data over USB using raw HID packets
# to the hardware monitor.
#
########################################################

import hid		#https://github.com/olsoneric/pyhidapi (forked from https://github.com/apmorton/pyhidapi)
import psutil	#https://github.com/giampaolo/psutil
import pynvml	#https://pypi.python.org/pypi/nvidia-ml-py/
import sensors	#https://pypi.python.org/pypi/PySensors/

import sys
from time import sleep

buf = list("CXXXMXXdd\xDFRXXDXX#GXXMXXdd\xDFWXXUXX")
dev = hid.Device(vid=5824, pid=1152)

def main():
	state = {
		"cpu": [], "memory": [],
		"gpu": [], "gpu_memory": [], "gpu_count": pynvml.nvmlDeviceGetCount(),
		"disk_read": [], "disk_write": [], "disk_read_last": psutil.disk_io_counters().read_bytes, "disk_write_last": psutil.disk_io_counters().write_bytes,
		"net_upload": [], "net_download": [], "net_upload_last": psutil.net_io_counters().bytes_sent, "net_download_last": psutil.net_io_counters().bytes_recv
	}
	
	while True:
		exception = False
		
		#CPU Usage:
		state["cpu"].append(int(psutil.cpu_percent() / 11));
		if (len(state["cpu"]) > 300):
			state["cpu"].pop(0)
		last60 = state["cpu"][-60:]
		
		buf[1] = writeBarGraph(state["cpu"][-1])
		buf[2] = writeBarGraph(sum(last60) / len(last60))
		buf[3] = writeBarGraph(sum(state["cpu"]) / len(state["cpu"]))
		
		#Memory Usage:
		state["memory"].append(int(psutil.virtual_memory().percent / 11))
		if (len(state["memory"]) > 60):
			state["memory"].pop(0)
		buf[5] = writeBarGraph(state["memory"][-1])
		buf[6] = writeBarGraph(sum(state["memory"]) / len(state["memory"]))

		#CPU Temperature:
		for chip in sensors.iter_detected_chips():
			for feature in chip:
				if (feature.label == "Physical id 0"):
					cpu_temp = int(feature.get_value())
					if (cpu_temp > 99):
						cpu_temp = 99
					buf[7] = str(cpu_temp / 10)
					buf[8] = str(cpu_temp % 10)
		
		#GPU Stuff
		gpuHandle = pynvml.nvmlDeviceGetHandleByIndex(0)	#If you have multiple GPUs, iterate over them here and average the results
		
		#GPU Usage:
		state["gpu"].append(int((15 - pynvml.nvmlDeviceGetPowerState(gpuHandle)) / 15.0 * 100) / 11)
		if (len(state["gpu"]) > 60):
			state["gpu"].pop(0)
		buf[18] = writeBarGraph(state["gpu"][-1])
		buf[19] = writeBarGraph(sum(state["gpu"]) / len(state["gpu"]))
		
		
		#GPU Memory Usage:
		gpu_memory = pynvml.nvmlDeviceGetMemoryInfo(gpuHandle)
		state["gpu_memory"].append(int(float(gpu_memory.used) * 100 / gpu_memory.total) / 11)
		if (len(state["gpu_memory"]) > 60):
			state["gpu_memory"].pop(0)
		buf[21] = writeBarGraph(state["gpu_memory"][-1])
		buf[22] = writeBarGraph(sum(state["gpu_memory"]) / len(state["gpu_memory"]))
		
		#GPU Temperature:
		gpu_temp = pynvml.nvmlDeviceGetTemperature(gpuHandle, pynvml.NVML_TEMPERATURE_GPU)
		if (gpu_temp > 99):
			gpu_temp = 99
		buf[23] = str(gpu_temp / 10)
		buf[24] = str(gpu_temp % 10)

		#Disk I/O:
		disk = psutil.disk_io_counters()

		state["disk_read"].append(disk.read_bytes - state["disk_read_last"])
		state["disk_read_last"] = disk.read_bytes
		state["disk_write"].append(disk.write_bytes - state["disk_write_last"])
		state["disk_write_last"] = disk.write_bytes

		if (len(state["disk_read"]) > 60):
			state["disk_read"].pop(0)
		if (len(state["disk_write"]) > 60):
			state["disk_write"].pop(0)
			
		buf[11] = writeBarGraph(scaleNetwork(state["disk_read"][-1]))
		buf[12] = writeBarGraph(scaleNetwork(sum(state["disk_read"]) / len(state["disk_read"])))
		buf[27] = writeBarGraph(scaleNetwork(state["disk_write"][-1]))
		buf[28] = writeBarGraph(scaleNetwork(sum(state["disk_write"]) / len(state["disk_write"])))
		
		
		#Network I/O:
		net = psutil.net_io_counters()
		
		state["net_download"].append(net.bytes_recv - state["net_download_last"])
		state["net_download_last"] = net.bytes_recv
		state["net_upload"].append(net.bytes_sent - state["net_upload_last"])
		state["net_upload_last"] = net.bytes_sent
		
		if (len(state["net_download"]) > 60):
			state["net_download"].pop(0)
		if (len(state["net_upload"]) > 60):
			state["net_upload"].pop(0)
			
		buf[14] = writeBarGraph(scaleNetwork(state["net_download"][-1]))
		buf[15] = writeBarGraph(scaleNetwork(sum(state["net_download"]) / len(state["net_download"])))
		buf[30] = writeBarGraph(scaleNetwork(state["net_upload"][-1]))
		buf[31] = writeBarGraph(scaleNetwork(sum(state["net_upload"]) / len(state["net_upload"])))

		#Exception report
		if (exception):
			buf[16] = '*'
		else:
			buf[16] = ' '

		sendData()

		sleep(1)

def sendData():
	dev.write(''.join(buf))

def scaleNetwork(value):
	if (value > 0x40000000):
		return 9
	elif (value > 0x20000000):
		return 8
	elif (value > 0x10000000):
		return 7
	elif (value > 0x8000000):
		return 6
	elif (value > 0x4000000):
		return 5
	elif (value > 0x2000000):
		return 4
	elif (value > 0x1000000):
		return 3
	elif (value > 0x800000):
		return 2
	elif (value > 0x400000):
		return 1
	elif (value > 0x200000):
		return 0

def writeBarGraph(value):
	if (value <= 0):
		return ' '
	elif (value >= 8):
		return '\xff'
	else:
		return chr(value)

########## Main startup hooks here ##########
if (__name__=="__main__"):
	try:
		pynvml.nvmlInit()
		sensors.init()
		main()
	finally:
		pynvml.nvmlShutdown()
		sensors.cleanup()
		dev.close()

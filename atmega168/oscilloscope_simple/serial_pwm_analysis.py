#!/opt/local/bin/python2.6
#
# Reads the AVR output, and keeps track of values assuming the signal is PWM output.  Will report period, phase, etc.
#
###################

import serial

ser = serial.Serial("/dev/tty.usbserial-FTE0U36U", 76800)

last = 0x0

period_sum = 0
phase_sum = 0
count = 0
max = 0
min = 100000000

state_start_time = 0
state_phase_end_time = 0
state_period_end_time = 0

while True:
	while ord(ser.read()) != 0x7F:
		p = 0

	time = ord(ser.read())
	time = (time << 8) + ord(ser.read())
	time = (time << 8) + ord(ser.read())
	time = (time << 8) + ord(ser.read())

	value = time & 0x1
	time = time & ~0x1
	
	if value == 1:
		state_period_end_time = time
		if state_start_time != 0 and state_phase_end_time != 0:
			count = count + 1
			period = time - state_start_time
			phase = state_phase_end_time - state_start_time

			period_sum = period_sum + period
			phase_sum = phase_sum + phase
			print(str(phase) + "\t" + str(period) + "\t" + str(phase_sum / count) + "\t" + str(period_sum / count))
		state_start_time = time		#mark new start time
		state_phase_end_time = 0	#reset phase end time

	elif value == 0:
		state_phase_end_time = time



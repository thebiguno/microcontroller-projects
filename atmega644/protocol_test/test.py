#!/usr/bin/env python2.6

import serial, sys

START = 0x07e
ESCAPE = 0x7d
XON = 0x11
XOFF = 0x13

MAX_SIZE = 40

buf = dict()
b = 0
err = False
esc = False
pos = 0
len = 0
cmd = 0
chk = 0x00

ser = serial.Serial("/dev/tty.usbserial-A8007UF6", 57600)

def dispatch(cmd, buf, len):
	sys.stdout.write(hex(cmd))
	sys.stdout.write(":\n")
	for x in range(0,len-1,4):
		printfl(buf, x)

def printfl(buf,x):
	print("%0.2X" % buf[x+3] + "%0.2X" % buf[x+2] + "%0.2X" % buf[x+1] + "%0.2X" % buf[x+0])

while True:
	b = ord(ser.read())
	if b == -1:
		sys.stdout.write("end of stream\n")
		break
	if err and b == START:
		sys.stdout.write("recover from error\n")
		err = False
		pos = 0
	elif err:
		continue
	
	if pos > 0 and b == START:
		sys.stdout.write("unexpected start of frame\n")
		err = True
		continue
	
	if pos > 0 and b == ESCAPE:
		sys.stdout.write("unescape next byte\n")
		esc = True
	
	if esc:
		sys.stdout.write("unescape current byte\n")
		b = 0x20 ^ b
		esc = False
		
	if pos > 1:
		chk = (chk + b) & 0xFF
		
	if pos == 0:
		pos = pos + 1
	elif pos == 1:
		len = b
		pos = pos + 1
	elif pos == 2:
		cmd = b
		pos = pos + 1
	else:
		if pos > MAX_SIZE:
			sys.stdout.write("unexpected pos > MAX_SIZE\n")
		if pos == (len + 2):
			if (chk == 0xff):
				dispatch(cmd, buf, len)
			else:
				sys.stdout.write("invalid checksum\n")
				err = True
			pos = 0
			chk = 0
		else:
			buf[pos - 3] = b;
			pos = pos + 1;


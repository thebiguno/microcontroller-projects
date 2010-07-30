#!/opt/local/bin/python2.6
#
# Tries to convert CSV PWM signals into data
#
###################

import sys

value1 = 0
time1 = 0
value2 = 0
time2 = 0
value3 = 0
time3 = 0

data = sys.stdin.readlines()

for line in data:
	splitLine = line.split(",")
	if value1 != splitLine[4]:
		timeDiff = int(splitLine[0]) - time1
		print(timeDiff)
		time1 = int(splitLine[0])
		value1 = int(splitLine[4])

	if value2 != splitLine[3]:
		timeDiff = int(splitLine[0]) - time2
		print(timeDiff)
		time2 = int(splitLine[0])
		value2 = int(splitLine[3])

	if value3 != splitLine[2]:
		timeDiff = int(splitLine[0]) - time3
		print(timeDiff)
		time3 = int(splitLine[0])
		value3 = int(splitLine[2])



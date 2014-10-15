# This file is the single location which defines what hardware is available on the system.
# Override lines to change PCB revision, magentometer, distance sensor, etc.  Comment
# out a given line to revert to default value.
########################

#PCB Revision.  Set to 1 for 1.x, 2 for 2.x, etc.
#Defaults to 2
#PCB_REVISION=2

#Is a magnetometer on board?  Set to 1 for yes, 0 for no.
#Defaults to 1
#MAGNETOMETER=1

#Is a distance sensor on board?  Set to 1 for yes, 0 for no.
#Defaults to 1
#DISTANCE_SENSOR=1

#Manually set the F_CPU speed.  Only do this if you are using a non-standard value
# (i.e. something other than 20MHz for Rev2 boards, or 12MHz for Rev1 boards).
#Defaults to the nominal value for the specified board revision.
#F_CPU=20000000

#Manually set the TWI_FREQ speed.  Only do this if you are having problems with the default
# (most likely happening if you don't have high enough pull-ups on SDA / SCL).
#Defaults to 400000L
#TWI_FREQ=400000L

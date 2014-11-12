Raw HID sample program; meant to be used with samples/avr/atmega32u2/usb_rawhid to demonstrate reading and writing bytes.

To run this, use the following commands:

javac -cp hidapi-1.1.jar Main.java
java -cp hidapi-1.1.jar:. Main

(On my 64 bit Linux system you need to run the java command as root with sudo or else you cannot open a device... see https://code.google.com/p/javahidapi/issues/detail?id=21 for the issue tracker).
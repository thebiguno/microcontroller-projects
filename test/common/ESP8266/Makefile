all:
	g++ -pedantic -Os -Wall \
	-I../../../inc/common/Stream/ \
	-I../../../inc/common/ESP8266/ \
	-I./ \
	-o test.out \
	Main.cpp \
	../../../inc/common/Stream/Stream.cpp \
	../../../inc/common/Stream/ArrayStream.cpp \
	../../../inc/common/ESP8266/ESP8266.cpp \
	../../../inc/common/ESP8266/ESP8266Socket.cpp \
	MockStream.cpp \
	ESP8266Test.cpp
	./test.out

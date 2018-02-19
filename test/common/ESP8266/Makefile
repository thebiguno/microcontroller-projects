all:
	g++ -pedantic -Os -Wall \
	-DESP8266_DEBUG \
	-I../../../inc/common/Stream/ \
	-I../../../inc/common/ESP8266/ \
	-I./ \
	-o test.out \
	Main.cpp \
	../../../inc/common/Stream/Stream.cpp \
	../../../inc/common/Stream/ArrayStream.cpp \
	../../../inc/common/ESP8266/ESP8266.cpp \
	../../../inc/common/ESP8266/ESP8266Stream.cpp \
	MockStream.cpp \
	ESP8266Test.cpp
	./test.out

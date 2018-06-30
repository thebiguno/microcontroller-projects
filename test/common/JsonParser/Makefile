all:
	g++ -pedantic -Os -Wall  -std=c++11 \
	-I../../../inc/common/dcutil/ \
	-I../../../inc/common/Stream/ \
	-I../../../inc/common/Web/ \
	-I../../../inc/common/Json/ \
	-I../Test/ \
	-I./ \
	-o test.out \
	Main.cpp \
	../../../inc/common/dcutil/dcstring.c \
	../../../inc/common/Stream/Stream.cpp \
	../../../inc/common/Stream/ArrayStream.cpp \
	../../../inc/common/Json/JsonParser.cpp \
	../Test/MockStream.cpp \
	../Test/Test.cpp \
	JsonParserTest.cpp
	./test.out

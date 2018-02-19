all:
	g++ -pedantic -Os -Wall  -std=c++11 \
	-I../../../inc/common/dcutil/ \
	-I../../../inc/common/Stream/ \
	-I../../../inc/common/Web/ \
	-I./ \
	-o test.out \
	Main.cpp \
	../../../inc/common/dcutil/dcstring.c \
	../../../inc/common/Stream/Stream.cpp \
	../../../inc/common/Stream/ArrayStream.cpp \
	../../../inc/common/Web/WebStream.cpp \
	MockStream.cpp \
	WebTest.cpp
	./test.out

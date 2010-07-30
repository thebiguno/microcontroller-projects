COMPILER=gcc

CFLAGS=-pedantic -Os -Wall -std=gnu99

PROJECT=pid_test
SOURCES=pid_test.c pid.c

# Default target.
all: clean build

build: $(PROJECT).out

$(PROJECT).out: $(SOURCES)
	$(COMPILER) $(CFLAGS) -I./ -o $(PROJECT).out $(SOURCES)


clean:
	rm -f *.o
	rm -f $(PROJECT).out

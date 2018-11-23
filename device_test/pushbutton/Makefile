CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld

CFLAGS = -DNO_DEBUG
LDFLAGS = -static

default: pushbuttontest

pushbuttontest: pushbuttontest.o
	$(CC) $(LDFLAGS) -o pushbuttontest pushbuttontest.o

pushbuttontest.o: pushbuttontest.c
	$(CC) $(CFLAGS) -c pushbuttontest.c -o pushbuttontest.o

clean:
	rm -f pushbuttontest pushbuttontest.o

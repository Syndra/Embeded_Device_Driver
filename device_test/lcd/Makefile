CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld

CFLAGS = -DNO_DEBUG
LDFLAGS = -static

default: lcdtest

lcdtest: lcdtest.o
	$(CC) $(LDFLAGS) -o lcdtest lcdtest.o

lcdtest.o: lcdtest.c
	$(CC) $(CFLAGS) -c lcdtest.c -o lcdtest.o

clean:
	rm -f lcdtest lcdtest.o

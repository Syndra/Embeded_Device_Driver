CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld

CFLAGS = -DNO_DEBUG
LDFLAGS = -static

default: buzzertest

buzzertest: buzzertest.o
	$(CC) $(LDFLAGS) -o buzzertest buzzertest.o

buzzertest.o: buzzertest.c
	$(CC) $(CFLAGS) -c buzzertest.c -o buzzertest.o

clean:
	rm -f buzzertest buzzertest.o

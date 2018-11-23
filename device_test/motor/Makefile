CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld

CFLAGS = -DNO_DEBUG
LDFLAGS = -static

default: motortest

motortest: motortest.o
	$(CC) $(LDFLAGS) -o motortest motortest.o

motortest.o: motortest.c
	$(CC) $(CFLAGS) -c motortest.c -o motortest.o

clean:
	rm -f motortest motortest.o 

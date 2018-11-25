CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld

CFLAGS = -DNO_DEBUG
LDFLAGS = -static

default: motortest

motortest: motortest.o
	$(CC) $(LDFLAGS) -o motortest motortest.o

motortest.o: motortest.c
	$(CC) $(CFLAGS) -c motortest.c -o motortest.o

motorspeedtest: motorspeedtest.o
	$(CC) $(LDFLAGS) -o motorspeedtest motorspeedtest.o

motorspeedtest.o: motorspeedtest.c
	$(CC) $(CFLAGS) -c motorspeedtest.c -o motorspeedtest.o

clean:
	rm -f motortest motortest.o
	rm -f motorspeedtest motorspeedtest.o

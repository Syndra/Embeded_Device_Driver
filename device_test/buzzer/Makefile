CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld

CFLAGS = -DNO_DEBUG
LDFLAGS = -static

default: ledtest

ledtest: ledtest.o 
	$(CC) $(LDFLAGS) -o ledtest ledtest.o

ledtest.o: ledtest.c
	$(CC) $(CFLAGS) -c ledtest.c -o ledtest.o

clean:
	rm -f ledtest ledtest.o 


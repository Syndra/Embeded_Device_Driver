CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld

CFLAGS = -DNO_DEBUG
LDFLAGS = -static


default: dottest

7segtest: dottest.o
	$(CC) $(LDFLAGS) -o dottest dottest.o

7segtest.o: dottest.c
	$(CC) $(CFLAGS) -c dottest.c -o dottest.o

clean:
	rm -f dottest.o dottest

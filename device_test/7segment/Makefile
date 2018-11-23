CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld

CFLAGS = -DNO_DEBUG
LDFLAGS = -static


default: 7segtest

7segtest: 7segtest.o
	$(CC) $(LDFLAGS) -o 7segtest 7segtest.o

7segtest.o: 7segtest.c
	$(CC) $(CFLAGS) -c 7segtest.c -o 7segtest.o

clean:
	rm -f 7segtest.o ledtest

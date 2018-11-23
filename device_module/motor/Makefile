CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld
obj-m := motor.o
KDIR := /home/lenny/kernelsource/achroimx_kernel
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f *.ko
	rm -f *.o
	rm -f *.mod.*
	rm -f .*.cmd

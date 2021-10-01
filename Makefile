obj-m += simple.o
UNAME=4.15.0-151-generic
all:
	make -C /lib/modules/$(UNAME)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(UNAME)/build M=$(PWD) clean
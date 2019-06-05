
obj-m :=character_device_driver_dynamically_major_minor.o


KDIR = /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR)  M=$(shell pwd) modules

clean:
	make -C $(KDIR) M=$(shell pwd) clean

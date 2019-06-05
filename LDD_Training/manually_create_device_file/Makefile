
obj-m :=manually_create_device_file.o


KDIR = /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR)  M=$(shell pwd) modules

clean:
	make -C $(KDIR) M=$(shell pwd) clean

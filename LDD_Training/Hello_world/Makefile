
obj-m :=Hello_world.o

value=$(shell export 10)


KDIR = /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR)  M=$(shell pwd) modules

clean:
	make -C $(KDIR) M=$(shell pwd) clean

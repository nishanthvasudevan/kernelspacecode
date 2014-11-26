obj-m += filename.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules	
install:
	sudo insmod filename.ko
uninstall:
	sudo rmmod filename
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


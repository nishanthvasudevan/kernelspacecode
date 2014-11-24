obj-m += filename.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules	
install:
	sudo insmod pscode.ko
uninstall:
	sudo rmmod pscode
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


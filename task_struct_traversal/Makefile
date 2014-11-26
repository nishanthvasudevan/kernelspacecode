obj-m += tstruct.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules	
install:
	sudo insmod tstruct.ko
uninstall:
	sudo rmmod tstruct
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


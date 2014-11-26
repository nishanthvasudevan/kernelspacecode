obj-m += kthreadwork.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules	
install:
	sudo insmod kthreadwork.ko
uninstall:
	sudo rmmod kthreadwork
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


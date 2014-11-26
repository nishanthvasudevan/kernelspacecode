obj-m += listas.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules	
install:
	sudo insmod listas.ko
uninstall:
	sudo rmmod listas
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


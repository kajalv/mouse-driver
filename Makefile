obj-m = dev_driver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc tester.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm a.out

tester: tester.c
	cc -o tester tester.c

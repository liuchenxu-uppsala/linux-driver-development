KERNELDIR := /home/george/linux/AtomKernel
CURRENT_PATH := $(shell pwd)
obj-m := chrdevbase.o
ccflags-y += -fno-pic
build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean

# make -C /home/george/linux/AtomKernel M=$(pwd) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules
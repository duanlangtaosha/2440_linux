KERN_DIR = /home/yang/1_linux_learn/resource/linux-2.6.22.6

# -C 先切换到KERN_DIR变量的目录
all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

#编译成模块
obj-m	+= led_drv.o led_dev.o


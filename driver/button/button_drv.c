#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>

/* class 相关的类的函数是通过extern 引用的方式放在device.h文件中 */
#include <linux/device.h>
#include <asm-arm/io.h>
#include <asm-arm/uaccess.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>

static int __s3c2440_button_open(struct inode *inode, struct file *file)
{
    printk("button open!\n");    

    /* 必须要加上return，如果将return去掉可能引发段错误 */
    return 0;
}

static int __s3c2440_button_write(struct file *p_file, const char __user *p_buf, size_t count, loff_t *p_pos)
{
    int val = 0, i = 0;
    uint8_t copy_num = 0;


    return 0;
}

static struct file_operations first_drv_fops = {
    .owner  =   THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量
                                 * 当模块make的时候会自动生成一个xxx.mod.c的文件，该文件
                                 * 中定义了__this_module的变量。
                                 */
    .open   =   __s3c2440_button_open,     
    .write  =   __s3c2440_button_write,       
};


static int __init __s3c2440_button_init(void)
{
    int minor = 0;
    printk("button_init!\n");

  
    return 0;
}

static void __exit __s3c2440_button_exit(void)
{
    int i = 0;

    printk("button_exit!\n");

}


/* insmod 时调用__s3c2440_button_init */
module_init(__s3c2440_button_init);

/* rmmod 时调用__s3c2440_button_exit */
module_exit(__s3c2440_button_exit);


/* 描述驱动程序的一些信息，不是必须的 */
MODULE_AUTHOR("duanlangtaosha");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("S3C2410/S3C2440 BUTTON Driver");
MODULE_LICENSE("GPL");



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


/*
*   天嵌2440的LED对应的接口
*   LED1 ----- GPB5
*   LED2 ----- GPB6
*   LED3 ------GPB7
*   LED4 ----- GPB8
*
*
*   GPBCON   0x56000010
*   GPBDAT   0x56000014
*   GPBUP    0x56000018
*
*   添加功能：可以打开4盏LED灯, leds设备控制全部的LED灯，LED0 控制第1盏，.... LED3控制第3盏
*/



static int major = 0;
static struct class *led_class = NULL; 
static struct class_device * leds_class_dev = NULL;
static struct class_device * led_class_dev[4];
static volatile unsigned long *gpbcon = NULL;
static volatile unsigned long *gpbdat = NULL;
static volatile unsigned long *gpbup = NULL;
static volatile unsigned long *clkcon = NULL;



static int __s3c2440_led_open(struct inode *inode, struct file *file)
{
    printk("led open!\n");    

    /* 获取次设备号，根据应用层所打开的设备文件来控制相应的LED灯 */
    int minor = MINOR(inode->i_rdev); //MINOR(inode->i_cdev);

    switch (minor) {
       /* leds */
       case  0:
            /* 配置引脚为输出 */
            *gpbcon &= ~((0x3<<(5*2)) | (0x3<<(6*2)) | (0x3<<(7*2)) | (0x3<<(8*2)));
            *gpbcon |= ((0x1<<(5*2)) | (0x1<<(6*2)) | (0x1<<(7*2)) | (0x1<<(8*2)));
            *gpbup  = ~((0x3<<(5*2)) | (0x3<<(6*2)) | (0x3<<(7*2)) | (0x3<<(8*2)));
             break;
       case 1:
            *gpbcon &= ~((0x3 << (5 * 2)));
            *gpbcon |= ((0x1 << (5 * 2)));
            *gpbup  = ~(0x3<<(5 * 2));
             break;
       case 2:
            *gpbcon &= ~((0x3 << (6 * 2)));
            *gpbcon |=  ((0x1 << (6 * 2)));
            *gpbup  = ~(0x3<<(6 * 2));
             break;

       case 3:
            *gpbcon &= ~((0x3 << (7 * 2)));
            *gpbcon |=  ((0x1 << (7 * 2)));
            *gpbup  = ~(0x3<<(7 * 2));
             break;

       case 4:
            *gpbcon &= ~((0x3 << (8 * 2)));
            *gpbcon |=  ((0x1 << (8 * 2)));
            *gpbup  = ~(0x3<<(8 * 2));
             break;

    }

    /* 必须要加上return，如果将return去掉可能引发段错误 */
    return 0;
}

static int __s3c2440_led_write(struct file *p_file, const char __user *p_buf, size_t count, loff_t *p_pos)
{
    int val = 0, i = 0;
    uint8_t copy_num = 0;

    /* 通过file可以找到设备号，这里找的是次设备号，struct file的定义在linux/fs.h中
     * f_dentry是一个宏 #define f_dentry	 f_path.dentry ，这个宏还是在struct file的结构体中
     * dentry的定义是在linux/dcache中定义的。所以说这个和open的驱动中获取设备号的方式有点不同。
     */
    int minor = MINOR(p_file->f_dentry->d_inode->i_rdev);
    
    printk("%d\n", minor);
    
    printk("led write!\n");

    /* 用户空间向内核空间传递数据  ，copy_to_user 内核空间向用户空间传递数据 */
    copy_num = copy_from_user(&val, p_buf, count);

    /* 如果拷贝的值不对则返回-1 */
    if (copy_num == count) {
        return -1;
    }

    switch (minor) {

        case 0:
        {
            s3c2410_gpio_setpin(S3C2410_GPB5, (val & 0x1));
            s3c2410_gpio_setpin(S3C2410_GPB6, (val & 0x1));
            s3c2410_gpio_setpin(S3C2410_GPB7, (val & 0x1));
            s3c2410_gpio_setpin(S3C2410_GPB8, (val & 0x1));
            break;
        }
        case 1:
        {
            s3c2410_gpio_setpin(S3C2410_GPB5, (val & 0x1));
            break;
        }
        case 2:
        {
            s3c2410_gpio_setpin(S3C2410_GPB6, (val & 0x1));
            break;
        }
        case 3:
        {
            s3c2410_gpio_setpin(S3C2410_GPB7, (val & 0x1));
            break;
        }
        case 4:
        {
            s3c2410_gpio_setpin(S3C2410_GPB8, (val & 0x1));
            break;
        }
    }

//    if (1 == val) {
//        /* 点灯 */
//    *gpbdat|= (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8);
//    printk("led on !\n");
//    } else {
//        /* 灭灯 */
//    *gpbdat &= ~((1 << 5) | (1 << 6) | (1 << 7) | (1 << 8));
//    printk("led off !\n");
//    }

    printk("*clkcon %x\n", *clkcon);
    printk("*gpbcon %x\n", *gpbcon);
    printk("*gpbdat %x\n", *gpbdat);

    /* 必须要加上return，如果将return去掉可能引发段错误 */
    return 0;
}

static struct file_operations first_drv_fops = {
    .owner  =   THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 
                                 * 当模块make的时候会自动生成一个xxx.mod.c的文件，该文件
                                 * 中定义了__this_module的变量。
                                 */
    .open   =   __s3c2440_led_open,     
	.write	=	__s3c2440_led_write,	   
};


static int __s3c2440_led_init(void)
{
    int i = 0;
    printk("led_init!\n");

    //当主设备号写0，则系统会自动给分配一个主设备号，会在主设备号数组中找一个空缺
    major = register_chrdev(0, "led_drv", &first_drv_fops); // 注册, 告诉内核

    led_class =class_create(THIS_MODULE, "led_drv");
    leds_class_dev = class_device_create(led_class, NULL, MKDEV(major, 0), NULL, "leds"); /* 这个leds就是在/dev/leds 中的设备了 */

    /* 创建出led0 ~led3 这四个设备 */
    for (i = 0; i < 4; i++) {
        led_class_dev[i] = class_device_create(led_class, NULL, MKDEV(major, i + 1), NULL, "leds[%d]", i);

    }

    /* 物理地址到虚拟地址的映射 */
    gpbcon = (volatile unsigned long *)ioremap(0x56000010, 16);
    clkcon = (volatile unsigned long *)ioremap(0x4c00000c, 16);
    gpbdat = gpbcon + 1;
    gpbup  = gpbdat + 1;
    printk("gpbcon %x\n", (unsigned int)gpbcon);
    printk("gpbdat %x\n", (unsigned int)gpbdat);
    printk("clkcon %x\n", (unsigned int)clkcon);
    printk("*clkcon %x\n", *clkcon);
    printk("*gpbcon %x\n", *gpbcon);
    printk("*gpbdat %x\n", *gpbdat);

    *gpbdat|= (5 << 1) | (6 << 1) | (7 << 1) | (8 << 1);

    return 0;
}

static void __s3c2440_led_exit(void)
{
    int i = 0;

    /* 去除驱动的注册  */
    unregister_chrdev(100, "led_drv"); // 卸载

    class_device_unregister(leds_class_dev);

    for (i = 0; i < 4; i++) {
        class_device_unregister(led_class_dev[i]);
    }

    /* 释放创建的类 */
    class_destroy(led_class);

    /* 释放虚拟地址的映射 */
    iounmap(gpbcon);
    printk("led_exit!\n");

}



module_init(__s3c2440_led_init);
module_exit(__s3c2440_led_exit);

MODULE_LICENSE("GPL");



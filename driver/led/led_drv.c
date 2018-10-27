#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
//#include <linux/init.h>


static int major = 0;

static int __s3c2440_led_open(struct inode *inode, struct file *file)
{
    printk("led open!\n");


    /* 必须要加上return，如果将return去掉可能引发段错误 */
    return 0;
}

static int __s3c2440_led_write()
{
    printk("led write!\n");

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
    printk("s3c2440_led_init!\n");

    //当主设备号写0，则系统会自动给分配一个主设备号，会在主设备号数组中找一个空缺
    register_chrdev(100, "led_drv", &first_drv_fops); // 注册, 告诉内核
    return 0;
}

static void __s3c2440_led_exit(void)
{
    
    unregister_chrdev(100, "led_drv"); // 卸载
    printk("__s3c2440_led_exit!\n");

}



module_init(__s3c2440_led_init);
module_exit(__s3c2440_led_exit);

MODULE_LICENSE("GPL");



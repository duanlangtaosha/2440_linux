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
#include <linux/platform_device.h>


static int led_release(struct device * dev)
{
	printk("device rm!\n");
	return 0;
}


static struct resource led_resource[] = {
    [0] = {
        .start = 0x56000010,
        .end   = 0x56000010 + 8 - 1,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = 5,
        .end   = 5,
        .flags = IORESOURCE_IRQ,
    }

};


static struct platform_device leds_device = {
	.name = "leds",
	.id = -1,
	.num_resources = ARRAY_SIZE(led_resource),
	.resource      = led_resource,
	.dev = {
		.release = led_release,
	}
};



static int __init __s3c2440_led_init(void)
{
	int major = 0;
#if 0
	major = register_chrdev(0, "led_drv", &first_drv_fops); // 注册, 告诉内核

	led_class =class_create(THIS_MODULE, "led_drv");
	if (IS_ERR(led_class)) {
		/* 判断led_class的地址是否有效 */
		return PTR_ERR(led_class);
	}

	leds_class_dev = class_device_create(led_class, NULL, MKDEV(major, 0), NULL, "led_test"); /* 这个leds就是在/dev/leds 中的设备了 */
#endif
	return platform_device_register(&leds_device);

}

static void __exit __s3c2440_led_exit(void)
{

	return platform_device_unregister(&leds_device);
}


/* insmod 时调用__s3c2440_led_init */
module_init(__s3c2440_led_init);

/* rmmod 时调用__s3c2440_led_exit */
module_exit(__s3c2440_led_exit);


/* 描述驱动程序的一些信息，不是必须的 */
MODULE_AUTHOR("duanlangtaosha");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("S3C2410/S3C2440 LED Driver");
MODULE_LICENSE("GPL");





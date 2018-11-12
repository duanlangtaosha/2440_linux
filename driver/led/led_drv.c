#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>

/* class ��ص���ĺ�����ͨ��extern ���õķ�ʽ����device.h�ļ��� */
#include <linux/device.h>
#include <asm-arm/io.h>
#include <asm-arm/uaccess.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>


/*
*   ��Ƕ2440��LED��Ӧ�Ľӿ�
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
*   ��ӹ��ܣ����Դ�4յLED��, leds�豸����ȫ����LED�ƣ�LED0 ���Ƶ�1յ��.... LED3���Ƶ�3յ
*/

#define LED_MAJOR  0
#define DEVICE_NAME "led"   /* ����ģʽ��ִ�С�cat /proc/devices����������豸���� */

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

    /* ��ȡ���豸�ţ�����Ӧ�ò����򿪵��豸�ļ���������Ӧ��LED�� */
    int minor = MINOR(inode->i_rdev); //MINOR(inode->i_cdev);

    switch (minor) {
       /* leds */
       case  0:
            /* ��������Ϊ��� */
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

    /* ����Ҫ����return�������returnȥ�����������δ��� */
    return 0;
}

static int __s3c2440_led_write(struct file *p_file, const char __user *p_buf, size_t count, loff_t *p_pos)
{
    int val = 0, i = 0;
    uint8_t copy_num = 0;

    /* ͨ��file�����ҵ��豸�ţ������ҵ��Ǵ��豸�ţ�struct file�Ķ�����linux/fs.h��
     * f_dentry��һ���� #define f_dentry	 f_path.dentry ������껹����struct file�Ľṹ����
     * dentry�Ķ�������linux/dcache�ж���ġ�����˵�����open�������л�ȡ�豸�ŵķ�ʽ�е㲻ͬ��
     */
    int minor = MINOR(p_file->f_dentry->d_inode->i_rdev);
    
    printk("%d\n", minor);
    
    printk("led write!\n");

    /* �û��ռ����ں˿ռ䴫������  ��copy_to_user �ں˿ռ����û��ռ䴫������ */
    copy_num = copy_from_user(&val, p_buf, count);

    /* ���������ֵ�����򷵻�-1 */
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

//    printk("*clkcon %x\n", *clkcon);
//    printk("*gpbcon %x\n", *gpbcon);
//    printk("*gpbdat %x\n", *gpbdat);

    /* ����Ҫ����return�������returnȥ�����������δ��� */
    return 0;
}

static struct file_operations first_drv_fops = {
    .owner  =   THIS_MODULE,    /* ����һ���꣬�������ģ��ʱ�Զ�������__this_module����
                                 * ��ģ��make��ʱ����Զ�����һ��xxx.mod.c���ļ������ļ�
                                 * �ж�����__this_module�ı�����
                                 */
    .open   =   __s3c2440_led_open,     
	.write	=	__s3c2440_led_write,	   
};


static int __init __s3c2440_led_init(void)
{
    int minor = 0;
    printk("led_init!\n");

    /* ע���ַ��豸
     * ����Ϊ���豸�š��豸���֡�file_operations�ṹ��
     * ���������豸�žͺ;����file_operations�ṹ��ϵ�����ˣ�
     * �������豸ΪLED_MAJOR���豸�ļ�ʱ���ͻ����s3c24xx_leds_fops�е���س�Ա����
     * LED_MAJOR������Ϊ0����ʾ���ں��Զ��������豸��
     */
    major = register_chrdev(LED_MAJOR, "led_drv", &first_drv_fops); // ע��, �����ں�

    /* �豸û��ע��ɹ� */
    if (major < 0) {
    	printk(DEVICE_NAME"can't register major number\n");
    	return major;
    }

    led_class =class_create(THIS_MODULE, "led_drv");
    if (IS_ERR(led_class)) {
        /* �ж�led_class�ĵ�ַ�Ƿ���Ч */
        return PTR_ERR(led_class);
    }

    leds_class_dev = class_device_create(led_class, NULL, MKDEV(major, 0), NULL, "leds"); /* ���leds������/dev/leds �е��豸�� */

    /* ������led0 ~led3 ���ĸ��豸 */
    for (minor = 0; minor < 4; minor++) {
        led_class_dev[minor] = class_device_create(led_class, NULL, MKDEV(major, minor + 1), NULL, "led%d", minor);

        /* IS_ERR ��⴫�ݽ�ȥ�ĵ�ַ�Ƿ��Ǵ���(unsigned long)(-4095) , ������ڷ���TRUE
         * ��IS_ERR�����档
         *
         * likely �� unlikelyֻ��һ�����Σ�if(likely(a > b)) �� if(a > b) ��Ч����һ����
         * ֻ�Ǽ���likely�ǳ���Ա���߱�������a > b Ϊ��Ŀ����Դ�һЩ���ڱ�������п��ܽ�����ǰ���
         * ���룬�Ӷ����cache�������ʣ���֮unlikely��if(unlikely(a > b)) �� if (a > b)
         * ��Ч��Ҳ��һ���ģ�ֻ��unlikely����߱�����,a > bΪ�ٵĿ����Դ�һЩ���������᲻�������֧
         * ������Ĵ�������һ��
         */
        if (unlikely(IS_ERR(led_class_dev[minor]))) {

            /* PTR_ERR �Ὣָ��ת��Ϊ��������(long)ptr */
            return PTR_ERR(led_class_dev[minor]);
        }
    }

    /* �����ַ�������ַ��ӳ�� */
    gpbcon = (volatile unsigned long *)ioremap(0x56000010, 16);
    clkcon = (volatile unsigned long *)ioremap(0x4c00000c, 16);
    gpbdat = gpbcon + 1;
    gpbup  = gpbdat + 1;
//    printk("gpbcon %x\n", (unsigned int)gpbcon);
//    printk("gpbdat %x\n", (unsigned int)gpbdat);
//    printk("clkcon %x\n", (unsigned int)clkcon);
//    printk("*clkcon %x\n", *clkcon);
//    printk("*gpbcon %x\n", *gpbcon);
//    printk("*gpbdat %x\n", *gpbdat);

    *gpbdat |= (5 << 1) | (6 << 1) | (7 << 1) | (8 << 1);

    return 0;
}

static void __exit __s3c2440_led_exit(void)
{
    int i = 0;

    /* ȥ��������ע��  */
    unregister_chrdev(100, "led_drv"); // ж��

    class_device_unregister(leds_class_dev);

    for (i = 0; i < 4; i++) {
        class_device_unregister(led_class_dev[i]);
    }

    /* �ͷŴ������� */
    class_destroy(led_class);

    /* �ͷ������ַ��ӳ�� */
    iounmap(gpbcon);
    printk("led_exit!\n");

}


/* insmod ʱ����__s3c2440_led_init */
module_init(__s3c2440_led_init);

/* rmmod ʱ����__s3c2440_led_exit */
module_exit(__s3c2440_led_exit);


/* �������������һЩ��Ϣ�����Ǳ���� */
MODULE_AUTHOR("duanlangtaosha");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("S3C2410/S3C2440 LED Driver");
MODULE_LICENSE("GPL");



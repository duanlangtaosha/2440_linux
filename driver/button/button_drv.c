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

static int __s3c2440_button_open(struct inode *inode, struct file *file)
{
    printk("button open!\n");    

    /* ����Ҫ����return�������returnȥ�����������δ��� */
    return 0;
}

static int __s3c2440_button_write(struct file *p_file, const char __user *p_buf, size_t count, loff_t *p_pos)
{
    int val = 0, i = 0;
    uint8_t copy_num = 0;


    return 0;
}

static struct file_operations first_drv_fops = {
    .owner  =   THIS_MODULE,    /* ����һ���꣬�������ģ��ʱ�Զ�������__this_module����
                                 * ��ģ��make��ʱ����Զ�����һ��xxx.mod.c���ļ������ļ�
                                 * �ж�����__this_module�ı�����
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


/* insmod ʱ����__s3c2440_button_init */
module_init(__s3c2440_button_init);

/* rmmod ʱ����__s3c2440_button_exit */
module_exit(__s3c2440_button_exit);


/* �������������һЩ��Ϣ�����Ǳ���� */
MODULE_AUTHOR("duanlangtaosha");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("S3C2410/S3C2440 BUTTON Driver");
MODULE_LICENSE("GPL");



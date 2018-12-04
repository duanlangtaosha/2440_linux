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
#include <linux/input.h>
#include <linux/irqreturn.h>
#include <linux/irq.h>
#include <asm-arm/irq.h>

struct pin_desc{
    int          irq;        /* �жϺ�    */
    char        *name;       /* ����     */
    unsigned int pin;        /* �ж�����   */
    unsigned int key_val;    /* ����ֵ  */
};


struct pin_desc pins_desc[4] = {
    {IRQ_EINT1,  "K1",  S3C2410_GPF1,   KEY_L},
    {IRQ_EINT4,  "K2",  S3C2410_GPF4,   KEY_S},
    {IRQ_EINT2,  "K3",  S3C2410_GPF2,   KEY_ENTER},
    {IRQ_EINT0,  "K4",  S3C2410_GPF0,   KEY_LEFTSHIFT},
};


/* �����豸ָ��  */
static struct input_dev  *buttons_dev = NULL;

/* ��������  ָ�� */
static struct pin_desc   *irq_pd;

/* ��ʱ��   */
static struct timer_list  buttons_timer;

/* �����жϴ�����  */
static irqreturn_t buttons_irq(int irq, void *dev_id)
{
    /* ������������ֵ��irq_pd , �������ʱ���ص�������ʹ�� */
    irq_pd = (struct pin_desc *)dev_id;

    /* �޸Ķ�ʱ����ֵ10ms, 10ms������жϻص� */
    mod_timer(&buttons_timer, jiffies + HZ / 100);

    return IRQ_RETVAL(IRQ_HANDLED);
}


/* ������ʱ���ص�����  */
static void buttons_timer_function(unsigned long data)
{
    struct pin_desc * pindesc = irq_pd;
    unsigned int      pinval;

    /* ���Ϊ���򷵻�  */
    if (!pindesc)
        return;

    /* ��ȡ��ǰ�ж����źŵ����ŵ�ֵ */
    pinval = s3c2410_gpio_getpin(pindesc->pin);

    /* �������Ϊ�ߵ�ƽ  */
    if (pinval) {
        /* ���Ͱ����¼� �����һ������: 0-�ɿ�, 1-���� */
        input_event(buttons_dev, EV_KEY, pindesc->key_val, 0);

        /* �����¼���ͬ���¼����������Ҫ��       */
        input_sync(buttons_dev);
    } else {
        /* ���� */
        input_event(buttons_dev, EV_KEY, pindesc->key_val, 1);
        input_sync(buttons_dev);
    }
}


static int __init __s3c2440_button_init(void)
{
    int minor = 0, i = 0;

    /* �൱��malloc    */
    buttons_dev = input_allocate_device();

    /* ���Ӹ�����,�ڼ���ģ�������input: Unspecified device as /class/input/input1
     * �������ֺ�����������input: keys as /class/input/input1
     */
    buttons_dev->name = "keys";
    /* ����������ϵͳ���豸�ṹ��  */

    /* ���������¼�  */
    set_bit(EV_KEY, buttons_dev->evbit);

    /* �����ظ��¼�, ������²�̧��ͻ�һֱ������Ӧ���¼� */
    set_bit(EV_REP, buttons_dev->evbit);

    /* ���Բ��� L,S,ENTER,LEFTSHIT 4�������¼� */
    set_bit(KEY_L, buttons_dev->keybit);
    set_bit(KEY_S, buttons_dev->keybit);
    set_bit(KEY_ENTER, buttons_dev->keybit);
    set_bit(KEY_LEFTSHIFT, buttons_dev->keybit);

    /* ע�ᰴ���¼�, �����ͻ��ͨ�������е� input_register_handlerƥ������ */
    input_register_device(buttons_dev);

    /* ��ʼ��һ����ʱ��, ���ڰ��������� */
    init_timer(&buttons_timer);

    /* ��Ӷ�ʱ���ص�����  */
    buttons_timer.function = buttons_timer_function;

    /* ����ʱ����ӵ�������,  ��add_timer��ʱ���ͻ��Զ��ܣ�����ʱ�䵽�˺��
     * ���Զ��Զ�����ʱ���Ļص�������������û������buttons_timer.expires����
     * ֵĬ��Ϊ0,��ֻ�е�jiffies��ֵ��Ϊ0��ͻ��Զ�����ص�������
     */
    add_timer(&buttons_timer);

    for (i = 0; i < 4; i++) {
        /* �����ж�,   �жϺ�             ���жϻص�����,        �жϴ���         ������                ���ص������Ĳ���*/
        request_irq(pins_desc[i].irq, buttons_irq, IRQT_BOTHEDGE, pins_desc[i].name, &pins_desc[i]);
    }

    printk("button_init!\n");

    return 0;
}

static void __exit __s3c2440_button_exit(void)
{
    int i = 0;

    for (i = 0; i < 4; i++) {
        /* �ͷ��ж�  */
        free_irq(pins_desc[i].irq, &pins_desc[i]);
    }

    /* ɾ����ʱ��  */
    del_timer(&buttons_timer);

    /* ��ע�ᰴ�������豸  */
    input_unregister_device(buttons_dev);

    /* �ͷŰ����豸,       �൱��free */
    input_free_device(buttons_dev);	

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



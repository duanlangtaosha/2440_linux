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
#include <linux/input.h>
#include <linux/irqreturn.h>
#include <linux/irq.h>
#include <asm-arm/irq.h>

struct pin_desc{
    int          irq;        /* 中断号    */
    char        *name;       /* 名字     */
    unsigned int pin;        /* 中断引脚   */
    unsigned int key_val;    /* 按键值  */
};


struct pin_desc pins_desc[4] = {
    {IRQ_EINT1,  "K1",  S3C2410_GPF1,   KEY_L},
    {IRQ_EINT4,  "K2",  S3C2410_GPF4,   KEY_S},
    {IRQ_EINT2,  "K3",  S3C2410_GPF2,   KEY_ENTER},
    {IRQ_EINT0,  "K4",  S3C2410_GPF0,   KEY_LEFTSHIFT},
};


/* 输入设备指针  */
static struct input_dev  *buttons_dev = NULL;

/* 引脚描述  指针 */
static struct pin_desc   *irq_pd;

/* 定时器   */
static struct timer_list  buttons_timer;

/* 按键中断处理函数  */
static irqreturn_t buttons_irq(int irq, void *dev_id)
{
    /* 将引脚描述赋值给irq_pd , 方便给定时器回调函数中使用 */
    irq_pd = (struct pin_desc *)dev_id;

    /* 修改定时器的值10ms, 10ms后调用中断回调 */
    mod_timer(&buttons_timer, jiffies + HZ / 100);

    return IRQ_RETVAL(IRQ_HANDLED);
}


/* 按键定时器回调函数  */
static void buttons_timer_function(unsigned long data)
{
    struct pin_desc * pindesc = irq_pd;
    unsigned int      pinval;

    /* 如果为空则返回  */
    if (!pindesc)
        return;

    /* 获取当前中断引脚号的引脚的值 */
    pinval = s3c2410_gpio_getpin(pindesc->pin);

    /* 如果引脚为高电平  */
    if (pinval) {
        /* 发送按键事件 ：最后一个参数: 0-松开, 1-按下 */
        input_event(buttons_dev, EV_KEY, pindesc->key_val, 0);

        /* 输入事件的同步事件，这个必须要有       */
        input_sync(buttons_dev);
    } else {
        /* 按下 */
        input_event(buttons_dev, EV_KEY, pindesc->key_val, 1);
        input_sync(buttons_dev);
    }
}


static int __init __s3c2440_button_init(void)
{
    int minor = 0, i = 0;

    /* 相当于malloc    */
    buttons_dev = input_allocate_device();

    /* 不加个名字,在加载模块后会出现input: Unspecified device as /class/input/input1
     * 加了名字后会出现正常的input: keys as /class/input/input1
     */
    buttons_dev->name = "keys";
    /* 申请输入子系统的设备结构体  */

    /* 产生按键事件  */
    set_bit(EV_KEY, buttons_dev->evbit);

    /* 产生重复事件, 如果按下不抬起就会一直发送相应的事件 */
    set_bit(EV_REP, buttons_dev->evbit);

    /* 可以产生 L,S,ENTER,LEFTSHIT 4个键盘事件 */
    set_bit(KEY_L, buttons_dev->keybit);
    set_bit(KEY_S, buttons_dev->keybit);
    set_bit(KEY_ENTER, buttons_dev->keybit);
    set_bit(KEY_LEFTSHIFT, buttons_dev->keybit);

    /* 注册按键事件, 这样就会和通用驱动中的 input_register_handler匹配起来 */
    input_register_device(buttons_dev);

    /* 初始化一个定时器, 用于按键的消抖 */
    init_timer(&buttons_timer);

    /* 添加定时器回调函数  */
    buttons_timer.function = buttons_timer_function;

    /* 将定时器添加到链表中,  当add_timer后定时器就会自动跑，当定时间到了后就
     * 会自动自动启定时器的回调函数。在这里没有设置buttons_timer.expires期望
     * 值默认为0,则只有当jiffies的值变为0后就会自动进入回调函数。
     */
    add_timer(&buttons_timer);

    for (i = 0; i < 4; i++) {
        /* 申请中断,   中断号             ，中断回调函数,        中断触发         ，名字                ，回调函数的参数*/
        request_irq(pins_desc[i].irq, buttons_irq, IRQT_BOTHEDGE, pins_desc[i].name, &pins_desc[i]);
    }

    printk("button_init!\n");

    return 0;
}

static void __exit __s3c2440_button_exit(void)
{
    int i = 0;

    for (i = 0; i < 4; i++) {
        /* 释放中断  */
        free_irq(pins_desc[i].irq, &pins_desc[i]);
    }

    /* 删除定时器  */
    del_timer(&buttons_timer);

    /* 反注册按键输入设备  */
    input_unregister_device(buttons_dev);

    /* 释放按键设备,       相当于free */
    input_free_device(buttons_dev);	

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



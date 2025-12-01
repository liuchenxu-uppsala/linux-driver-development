//
// Created by Chenx on 2025/11/23.
//

//
// Created by Chenx on 2025/11/10.
//
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/of_gpio.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/of_irq.h>
#include <linux/irq.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define LED_ON 0
#define LED_OFF 1
#define CMD_CLOSE_TIMER (_IO(0XEF, 0x1))
#define CMD_START_TIMER (_IO(0XEF, 0x2))
#define CMD_RESET_TIME_TIMER (_IO(0XEF, 0x3))
#define DEFAULT_INTERVAL 1000

static int gpioValue = 1;

typedef struct led_device_t {
    dev_t devt;
    struct cdev c_dev;
    struct class* device_class;
    struct device led_dev;
    struct device_node* device_node;
    int gpio;
    struct timer_list timer;
    long timer_interval;
    /**
     * 设备树中的按键KEY0
     **/
    dev_t devt_key;
    struct cdev c_dev_key;
    struct class* device_class_key;
    struct device key_dev;
    struct device_node* device_node_key;
    int gpio_key;
    int irq_number;
    int key_current_value;
};

struct led_device_t led_device;
struct file_operations options;

void timerFunction(unsigned long addr) {
    printk("timerFunction");
    struct led_device_t* device = (struct led_device_t*)addr;
    int key_value = gpio_get_value(device->gpio_key);
    // 按下了
    if (key_value == 0) {
        led_device.key_current_value = 0;
    } else {
        if (led_device.key_current_value == 0) {
            gpio_set_value(device->gpio,!gpioValue);
            gpioValue = !gpioValue;
            led_device.key_current_value = -1;
        }
    }

}

static irqreturn_t irq_deal(int num, void *dev) {
    printk("irq_deal");
    // int key_value = gpio_get_value(led_device.gpio_key);
    mod_timer(&led_device.timer,jiffies + msecs_to_jiffies(10));
    return IRQ_RETVAL(IRQ_HANDLED);
}

void init_device(void) {
    printk("init_device");
    led_device.device_node = of_find_node_by_path("/gpioled");
    led_device.gpio = of_get_named_gpio(led_device.device_node,"led-gpio",0);
    if (led_device.device_node == NULL) {
        printk("device_node NULL");
    }
    gpio_request(led_device.gpio, "led_device_gpio");
    gpio_direction_output(led_device.gpio,1);

    alloc_chrdev_region(&led_device.devt,0,1,"led_device");
    led_device.c_dev.owner = THIS_MODULE;
    cdev_init(&led_device.c_dev,&options);
    cdev_add(&led_device.c_dev,led_device.devt,1);
    led_device.device_class = class_create(THIS_MODULE,"dev_class");
    device_create(led_device.device_class,NULL,led_device.devt,NULL,"dev_device");

    /**
     * 获取按键KEY0
     */
    printk("init_key");
    led_device.device_node_key = of_find_node_by_path("/key-new");
    led_device.gpio_key = of_get_named_gpio(led_device.device_node_key,"key-gpio",0);
    gpio_request(led_device.gpio_key, "led_device_gpio_key");
    gpio_direction_input(led_device.gpio_key);

    /**
     * 设置中断相关
     */
    led_device.irq_number = irq_of_parse_and_map(led_device.device_node_key,0);
    request_irq(led_device.irq_number,irq_deal,IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING|IRQF_SHARED,"key_irq",&led_device);
    led_device.key_current_value = -1;

    init_timer(&led_device.timer);
    led_device.timer.function = timerFunction;
    led_device.timer.data = (unsigned long)&led_device;
    led_device.timer_interval = DEFAULT_INTERVAL;
}

static void initIrq(void) {

}

static int __init led_driver_init(void){
    printk("led_driver_init");
    init_device();
    initIrq();
    return 0;
}

static void __exit led_driver_exit(void){
    printk("led_driver_exit");
    device_destroy(led_device.device_class,led_device.devt);
    class_destroy(led_device.device_class);
    cdev_del(&led_device.c_dev);
    unregister_chrdev_region(led_device.devt,1);
    gpio_free(led_device.gpio);
    del_timer(&led_device.timer);
    free_irq(led_device.irq_number,NULL);
}

ssize_t read(struct file * f, char __user *buff, size_t n, loff_t *l) {
    printk("led_driver_read");
    return 0;
}

int open(struct inode * inode, struct file *f) {
    printk("led_driver_open");

    // f->private_data = &led_device;
    // gpio_set_value(led_device.gpio,gpioValue);
    // gpioValue = ~ gpioValue;
    // return 0;
}

ssize_t write(struct file * f, const char __user *buffer, size_t n, loff_t *l) {
    printk("led_driver_write");
    // char command[1];
    // copy_from_user(command,buffer,1);
    // if (command[0] == LED_OFF) {
    //     gpio_set_value(led_device.gpio,LED_ON);
    // } else {
    //     gpio_set_value(led_device.gpio,LED_OFF);
    // }
}

int release(struct inode * inode, struct file *f) {
    printk("led_driver_release");
    return 0;
}

long unlocked_ioctl (struct file * fd, unsigned int cmd, unsigned long param){
    printk("unlocked_ioctl");
    switch (cmd) {
        case CMD_CLOSE_TIMER:
            printk("unlocked_ioctl CMD_CLOSE_TIMER");
            del_timer(&led_device.timer);
            break;
        case CMD_START_TIMER:
            printk("unlocked_ioctl CMD_START_TIMER");
            mod_timer(&led_device.timer,jiffies + msecs_to_jiffies(DEFAULT_INTERVAL));
            break;
        case CMD_RESET_TIME_TIMER:
            printk("unlocked_ioctl CMD_RESET_TIME_TIMER");
            led_device.timer_interval = param;
            mod_timer(&led_device.timer,jiffies + msecs_to_jiffies(param));
            break;
    }
    return 0;
}

struct file_operations options = {
    .owner = THIS_MODULE,
    .open = open,
    .release = release,
    // .read = read,
    // .write = write,
    .unlocked_ioctl = unlocked_ioctl,
};

module_init(led_driver_init);
module_exit(led_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChenxuLiu");
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

#define LED_ON 0
#define LED_OFF 1

typedef struct led_device_t {
    dev_t devt;
    struct cdev c_dev;
    struct class* device_class;
    struct device led_dev;
    struct device_node* device_node;
    int gpio;
};

struct led_device_t led_device;
struct file_operations options;

void init_device(void) {
    printk("init_device");
    led_device.device_node = of_find_node_by_path("/gpioled");
    led_device.gpio = of_get_named_gpio(led_device.device_node,"led-gpio",0);
    gpio_direction_output(led_device.gpio,1);

    alloc_chrdev_region(&led_device.devt,0,1,"led_device");
    led_device.c_dev.owner = THIS_MODULE;
    cdev_init(&led_device.c_dev,&options);
    cdev_add(&led_device.c_dev,led_device.devt,1);
    led_device.device_class = class_create(THIS_MODULE,"dev_class");
    device_create(led_device.device_class,NULL,led_device.devt,NULL,"dev_device");
}

static int __init led_driver_init(void){
    printk("led_driver_init");
    init_device();
    return 0;
}

static void __exit led_driver_exit(void){
    printk("led_driver_exit");
    device_destroy(led_device.device_class,led_device.devt);
    class_destroy(led_device.device_class);
    cdev_del(&led_device.c_dev);
    unregister_chrdev_region(led_device.devt,1);
    gpio_free(led_device.gpio);
}

ssize_t read(struct file * f, char __user *buff, size_t n, loff_t *l) {
    printk("led_driver_read");
    return 0;
}

int open(struct inode * inode, struct file *f) {
    printk("led_driver_open");
    f->private_data = &led_device;
    return 0;
}

ssize_t write(struct file * f, const char __user *buffer, size_t n, loff_t *l) {
    printk("led_driver_write");
    char command[1];
    copy_from_user(command,buffer,1);
    if (command[0] == LED_OFF) {
        gpio_set_value(led_device.gpio,LED_ON);
    } else {
        gpio_set_value(led_device.gpio,LED_OFF);
    }
}

int release(struct inode * inode, struct file *f) {
    printk("led_driver_release");
    return 0;
}

struct file_operations options = {
    .owner = THIS_MODULE,
    .open = open,
    .release = release,
    .read = read,
    .write = write,
};

module_init(led_driver_init);
module_exit(led_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChenxuLiu");
//
// Created by Chenx on 2025/11/12.
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

#define BEEP_ON 0
#define BEEP_OFF 1

typedef struct {
    dev_t dev;
    struct cdev c_dev;
    struct class* beep_class;
    struct device beep_device;
    struct device_node* dn;
    int beep_gpio;
} beep_driver_t;

beep_driver_t beep_driver;
struct file_operations beep_fops;

static void init_beep_driver(void) {
    beep_driver.dn = of_find_node_by_path("/gpio_beep");
    beep_driver.beep_gpio = of_get_named_gpio(beep_driver.dn, "beep_gpio",0);
    gpio_direction_output(beep_driver.beep_gpio,1);

    alloc_chrdev_region(&beep_driver.dev,0,1,"beep_driver");
    beep_driver.c_dev.owner = THIS_MODULE;
    cdev_init(&beep_driver.c_dev, &beep_fops);
    cdev_add(&beep_driver.c_dev, beep_driver.dev, 1);

    beep_driver.beep_class = class_create(THIS_MODULE,"beep_driver");
    beep_driver.beep_device = *device_create(beep_driver.beep_class,NULL,beep_driver.dev,NULL,"beep_driver");
}

static int __init beep_driver_init(void){
    printk("beep_driver_init");
    init_beep_driver();
    return 0;
}

static void __exit beep_driver_exit(void){
    printk("beep_driver_exit");
}

static int open (struct inode *n, struct file *f) {
    printk("beep_driver_open\n");
    return 0;
}

static int release(struct inode * n, struct file *f) {
    printk("beep_driver_release\n");
    device_destroy(beep_driver.beep_class,beep_driver.dev);
    class_destroy(beep_driver.beep_class);
    cdev_del(&beep_driver.c_dev);
    unregister_chrdev_region(beep_driver.dev,1);
    gpio_free(beep_driver.beep_gpio);
    return 0;
}

static ssize_t read(struct file * f, char __user * buffer, size_t n, loff_t *lo) {
    printk("beep_driver_read\n");
}

static ssize_t write(struct file * f, const char __user * user_buffer, size_t n, loff_t *lo) {
    printk("beep_driver_write\n");
    char command[1];
    copy_from_user(&command[0],user_buffer,1);
    if (command[0]==BEEP_ON) {
        gpio_set_value(beep_driver.beep_gpio,1);
    } else {
        gpio_set_value(beep_driver.beep_gpio,0);
    }
}


struct file_operations beep_fops = {
    .owner = THIS_MODULE,
    .open = open,
    .release = release,
    .read = read,
    .write =  write,
};


module_init(beep_driver_init);
module_exit(beep_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChenxuLiu");
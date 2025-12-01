//
// Created by Chenx on 2025/11/26.
//
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
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#define MISCBEEP_MINOR		289
typedef struct {
    dev_t dev_num;
    struct cdev cdev;
    struct device_node* node;
    int gpio;
} miscdevice_t;

static miscdevice_t miscdevice_beep;

static int open(struct inode * node, struct file * f) {
    printk("Misc Device open");
    f->private_data = &miscdevice_beep;
    return 0;
}

static ssize_t write(struct file *f, const char __user * buffer, size_t cnt, loff_t * l) {
    miscdevice_t *dev_local = f->private_data;
    printk("Misc Device write");
    unsigned char databuf[1];
    copy_from_user(databuf,buffer,cnt);
    if (databuf[0]==0x00) {
        gpio_set_value(dev_local->gpio,0);
    } else {
        gpio_set_value(dev_local->gpio,1);
    }
    return 0;
}

static struct file_operations fops_misc = {
    .owner = THIS_MODULE,
    .open = open,
    .write = write,
};
struct miscdevice misc_device = {
    .fops = &fops_misc,
    .minor = MISCBEEP_MINOR,
    .name = "MiscDev"
};

int probe(struct platform_device *device) {
    int ret = 0;
    miscdevice_beep.node = of_find_node_by_path("/gpio_beep");
    miscdevice_beep.gpio = of_get_named_gpio(miscdevice_beep.node,"beep_gpio",0);
    printk("miscdevice.gpio:%d\n",miscdevice_beep.gpio);
    gpio_request(miscdevice_beep.gpio, "beep_gpio");
    gpio_direction_output(miscdevice_beep.gpio, 1);
    ret = misc_register(&misc_device);
    if(ret < 0){
        printk("misc device register failed!\r\n");
        return -EFAULT;
    }
    return 0;
}

int remove(struct platform_device *device) {
    misc_deregister(&misc_device);
    return 0;
}

static const struct of_device_id compatibles[] = {
    {.compatible = "chenxu_beep"},
    { /* Sentinel */ }
};

struct platform_driver driver = {
    .probe = probe,
    .remove = remove,
    .driver = {
        .name = "gpio_beep",
        .of_match_table = compatibles,
    },
};

static int __init miscbeep_init(void)
{
    return platform_driver_register(&driver);
}

static void __exit miscbeep_exit(void)
{
    platform_driver_unregister(&driver);
}

module_init(miscbeep_init);
module_exit(miscbeep_exit);
MODULE_AUTHOR("Chenxu");
MODULE_LICENSE("GPL");

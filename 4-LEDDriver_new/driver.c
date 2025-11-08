//
// Created by Chenx on 2025/11/4.
//
#include <stdio.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>

static char read_buffer[100];
static char write_buffer[100];
static char kerner_data[] = {"kernel data"};

#define MAJOR 200
#define DRIVER_NAME "chrdev0"
static struct file_operations chrdevbase_fops;

static int __init chrdevbase_init(void){
    printk("chrdevbase_init");
    register_chrdev(MAJOR,DRIVER_NAME,&chrdevbase_fops);
    return 0;
}

static void __exit chrdevbase_exit(void){
    unregister_chrdev(MAJOR,DRIVER_NAME);
    printk("chrdevbase_exit");
}

static ssize_t read(struct file * file, char __user * user_cache, size_t n, loff_t *loff) {
    int return_value = 0;
    memcpy(read_buffer,kerner_data,sizeof(kerner_data));
    return_value = copy_to_user(user_cache,read_buffer,n);
    if (return_value == 0) {
        printk("read kernel data success\n");
    } else {
        printk("read kernel data failed\n");
    }
    return 0;
}

ssize_t write(struct file * file, const char __user * user_cache, size_t n, loff_t * loff) {
    int return_value = 0;
    return_value = copy_from_user(write_buffer,user_cache,n);
    if (return_value == 0) {
        printk("write kernel data success %s\n",write_buffer);
    } else {
        printk("write kernel data failed\n");
    }
    return 0;
}

int open(struct inode * node, struct file *f) {
    printk("open\n");
    return 0;
}

int release(struct inode * node, struct file *f) {
    printk("release\n");
    return 0;
}

 static struct file_operations chrdevbase_fops = {
    .owner = THIS_MODULE,
    .read = read,
    .write = write,
    .open = open,
    .release = release,
};

module_init(chrdevbase_init);
module_exit(chrdevbase_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Uppsala");
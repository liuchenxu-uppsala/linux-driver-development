//
// Created by Chenx on 2025/11/5.
//
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
static int __init chrdevbase_init(void){
    printk("chrdevbase_init");
    //register_chrdev(MAJOR,DRIVER_NAME,&chrdevbase_fops);
    return 0;
}

static void __exit chrdevbase_exit(void){
    //unregister_chrdev(MAJOR,DRIVER_NAME);
    printk("chrdevbase_exit");
}
module_init(chrdevbase_init);
module_exit(chrdevbase_exit);
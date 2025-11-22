//
// Created by Chenx on 2025/11/9.
//
#include <linux/module.h>
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

#define MAJOR_LED 200
#define DRIVER_NAME_LED "led_driver"
#define CCG_R1 0x020C406C //gpio1 clock 时钟使能
static void __iomem *ccg_r1_iomem;
#define SW_MUX_CTL_PAD_GPIO1_IO03 0X020E0068 //GPIO复用
static void __iomem *sw_mux_ctl_pad_gpio1_io03_iomem;
#define SW_PAD_CTL_PAD_GPIO1_IO03 0x020E02F4
static void __iomem *sw_pad_ctl_pad_gpio1_io03_iomem;
#define GPIO1_GDIR 0x0209C004
static void __iomem  *gdir_gpio1_io03_iomem;
#define GPIO_DR 0x0209C000
static void __iomem *dr_gpio1_io03_iomem;

#define LED_ON 1
#define LED_OFF 0

#define NAME_MODULE "led_driver_new"
#define NUMBER_DRIVER_DEVICE 1

int open(struct inode * node, struct file * f);
static ssize_t read(struct file * f, char __user * user_cache, size_t n, loff_t *l);
static ssize_t write(struct file * f, const char __user * cache, size_t n, loff_t *l);
static int release(struct inode * node, struct file *f);

static struct file_operations led_driver_fops = {
    .owner = THIS_MODULE,
    .open = open,
    .read = read,
    .write = write,
    .release = release,
};

struct driver_info_t {
    dev_t dev_no;
    struct cdev c_dev;
    struct class* dev_class;
    struct device* dev_class_dev;
    struct device_node *device_node_dt;
};
struct driver_info_t driver_info;


static void register_dev(void);
static void read_config_device_tree(void);
static void read_config_device_tree(void) {
    driver_info.device_node_dt = of_find_node_by_path("/led_device_tree_based");
    struct property *property_compatible = of_find_property(driver_info.device_node_dt,"compatible",NULL);
    printk("property_compatible:%s\n",(char*)property_compatible->value);
    ccg_r1_iomem = of_iomap(driver_info.device_node_dt,0);
    sw_mux_ctl_pad_gpio1_io03_iomem = of_iomap(driver_info.device_node_dt,1);
    sw_pad_ctl_pad_gpio1_io03_iomem = of_iomap(driver_info.device_node_dt,2);
    dr_gpio1_io03_iomem = of_iomap(driver_info.device_node_dt,3);
    gdir_gpio1_io03_iomem = of_iomap(driver_info.device_node_dt,4);
}
static void init_gpio(void) {
    // CCG使能
    //ccg_r1_iomem = ioremap(CCG_R1,4);
    u32 ccg_r1_clock = readl(ccg_r1_iomem);
    ccg_r1_clock&=~(3<<26);
    ccg_r1_clock |= (3<<26);
    writel(ccg_r1_clock, ccg_r1_iomem);
    // GPIO1_IO03复用
    //sw_mux_ctl_pad_gpio1_io03_iomem = ioremap(SW_MUX_CTL_PAD_GPIO1_IO03,4);
    writel(5,sw_mux_ctl_pad_gpio1_io03_iomem);
    //
    //sw_pad_ctl_pad_gpio1_io03_iomem = ioremap(SW_PAD_CTL_PAD_GPIO1_IO03,4);
    writel(0X10B0,sw_pad_ctl_pad_gpio1_io03_iomem);
    //配置输出
    //gdir_gpio1_io03_iomem = ioremap(GPIO1_GDIR,4);
    u32 gdir_u32_value = readl(gdir_gpio1_io03_iomem);
    gdir_u32_value &=~(1<<3);
    gdir_u32_value |= (1<<3);
    writel(gdir_u32_value,gdir_gpio1_io03_iomem);
    // 配置LED关闭状态
    //dr_gpio1_io03_iomem = ioremap(GPIO_DR,4);
    u32 dr_u32_value = readl(dr_gpio1_io03_iomem);
    //dr_u32_value &=~(1<<3);
    dr_u32_value |= (1<<3);
    writel(dr_u32_value,dr_gpio1_io03_iomem);
}

static void register_dev(void) {
    printk("register_dev");
    alloc_chrdev_region(&driver_info.dev_no,0,NUMBER_DRIVER_DEVICE,NAME_MODULE);
    driver_info.c_dev.owner = THIS_MODULE;
    cdev_init(&driver_info.c_dev,&led_driver_fops);
    cdev_add(&driver_info.c_dev, driver_info.dev_no, NUMBER_DRIVER_DEVICE);
    driver_info.dev_class = class_create(THIS_MODULE,"led_driver");
    driver_info.dev_class_dev = device_create(driver_info.dev_class,NULL,driver_info.dev_no,NULL,"led_driver");
}

static int __init led_driver_init(void){
    printk("led_driver_init");
    register_dev();
    return 0;
}

static void __exit led_driver_exit(void){
    printk("led_driver_exit");
    device_destroy(driver_info.dev_class,driver_info.dev_no);
    class_destroy(driver_info.dev_class);
    cdev_del(&driver_info.c_dev);
    unregister_chrdev_region(driver_info.dev_no,NUMBER_DRIVER_DEVICE);
}

int open(struct inode * node, struct file * f) {
    printk("open_led_driver");
    read_config_device_tree();
    // 进行设备的初始化
    init_gpio();
    // register_dev();
    // f->private_data = &driver_info;
    return 0;
}

static ssize_t read(struct file * f, char __user * user_cache, size_t n, loff_t *l) {
    return 0;
}

static void switch_led_status(unsigned char led_status) {
    u32 dr_u32_value = readl(dr_gpio1_io03_iomem);
    if (led_status == LED_OFF) {
        // 设置为 1
        dr_u32_value |= (1<<3);
        writel(dr_u32_value,dr_gpio1_io03_iomem);
    } else {
        // 对应项设置为 0
        dr_u32_value &=~(1<<3);
        writel(dr_u32_value,dr_gpio1_io03_iomem);
    }
}

static ssize_t write(struct file * f, const char __user * cache, size_t n, loff_t *l) {
    char command[1];
    unsigned long result = copy_from_user(&command[0],cache,1);
    if (result < 0) {
        printk(KERN_ERR "Error in copy_from_user");
        return -1;
    } else {
        switch_led_status(command[0]);
        return 0;
    }
}

static int release(struct inode * node, struct file *f) {
    printk("led_driver_release");
    return 0;
}

module_init(led_driver_init);
module_exit(led_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChenxuLiu");

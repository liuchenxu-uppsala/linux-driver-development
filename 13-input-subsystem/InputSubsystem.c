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
#include <linux/input.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/of_irq.h>
#include <linux/irq.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

typedef struct {
    struct device_node *of_node;
    int gpio;
    struct timer_list timer;
    struct input_dev *input_device;
    int irq_num;
    int current_key_value;
} input_dev_t;

input_dev_t input;

irqreturn_t irq_handle(int num, void * arg) {
    input_dev_t* dev = (input_dev_t*)arg;
    mod_timer(&input.timer,jiffies + msecs_to_jiffies(50));
    return IRQ_RETVAL(IRQ_HANDLED);
}

void function_timer(unsigned long arg) {
    int current_keyvalue = gpio_get_value(input.gpio);
    if (current_keyvalue == 0) {
        input_report_key(input.input_device,KEY_0,1);
    } else {
        input_report_key(input.input_device,KEY_0,0);
    }
}

int input_subsystem_init(){
    input.of_node = of_find_node_by_path("/key-new");
    input.gpio = of_get_named_gpio(input.of_node,"key-gpio",0);
    gpio_request(input.gpio, "key-gpio");
    gpio_direction_input(input.gpio);

    //中断
    input.irq_num = irq_of_parse_and_map(input.of_node,0);
    request_irq(input.irq_num,irq_handle,IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING | IRQF_SHARED,"inputSubSystem",NULL);


    // 初始化 timer
    init_timer(&input.timer);
    input.timer.function = function_timer;


    input.current_key_value = -1;

    // input_dev
    input.input_device = input_allocate_device();
    //input.input_device->evbit
    __set_bit(EV_KEY, input.input_device->evbit);
    __set_bit(EV_REP, input.input_device->evbit);
    __set_bit(KEY_0,input.input_device->keybit);

    input_register_device(input.input_device);

    return 0;
}

void exit_input_subsystem(void){
    input_unregister_device(input.input_device);
    input_free_device(input.input_device);
}
module_init(input_subsystem_init)
module_exit(exit_input_subsystem)
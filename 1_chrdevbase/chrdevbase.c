#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/io.h>
static int __init chrdevbase_init(void)
{
	return 0;
}

static void __exit chrdevbase_exit(void)
{

}
/**
 * 模块入口和出口
 */
module_init(chrdevbase_init);
module_exit(chrdevbase_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChenxuLiu");
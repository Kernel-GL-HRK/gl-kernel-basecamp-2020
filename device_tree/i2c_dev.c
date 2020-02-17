#include <linux/init.h>
#include <linux/module.h>
//#include <linux/i2c.h>
#include <linux/kernel.h>

static int __init i2c_device_init(void)
{
    printk(KERN_INFO "hello form i2d device\n");
    return 0;
}

static void __exit i2c_device_exit(void)
{
    printk(KERN_INFO "i2c device exit\n");
}

module_init(i2c_device_init);
module_exit(i2c_device_exit);

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nick");
MODULE_DESCRIPTION("Timer");
MODULE_VERSION("0.01");


static int __init mod_init(void)
{
	return 0;
}

static void __exit mod_exit(void)
{

}

module_init(mod_init);
module_exit(mod_exit);  
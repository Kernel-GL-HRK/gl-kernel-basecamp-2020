#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/module.h>

static int __init memory_init(void)
{
	return 0;
}

static void __exit memory_exit(void)
{

}

module_init(memory_init);
module_exit(memory_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrii Revva");
MODULE_DESCRIPTION("Homework #4: Memory managment");

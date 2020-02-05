#include <linux/module.h>
#include <linux/time.h>

static struct timespec64 last_read;

static int __init mytimer_init(void)
{
	pr_info("mytimer: init\n");
	ktime_get_real_ts64(&last_read);
	return 0;
}

static void __exit mytimer_exit(void)
{
	pr_info("mytimer: exit\n");
}

module_init(mytimer_init);
module_exit(mytimer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrii Revva");
MODULE_DESCRIPTION("Homework #3: Time Management");

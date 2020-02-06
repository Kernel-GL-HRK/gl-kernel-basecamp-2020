#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

#define TIMEOUT 1000U

//static struct timer_list inter_timer;
static unsigned long sec = 0;

static ssize_t inter_show(struct class *class, struct class_attribute *attr,
			  char *buffer);

void inter_callback(struct timer_list *timer);

static struct class *class_timer = NULL;
CLASS_ATTR_RO(inter);

DEFINE_TIMER(inter_timer, inter_callback);

static ssize_t inter_show(struct class *class, struct class_attribute *attr,
			  char *buffer)
{
	mod_timer(&inter_timer, jiffies + msecs_to_jiffies(TIMEOUT));
	sprintf(buffer, "time = %lu s\n", sec);
	sec = 0;
	return strlen(buffer);
}

void inter_callback(struct timer_list *timer)
{
	mod_timer(&inter_timer, jiffies + msecs_to_jiffies(TIMEOUT));
	++sec;
	//printk(KERN_INFO "timer: callback\n");
}

static int timer_init(void)
{
	int ret;

	class_timer = class_create(THIS_MODULE, "timer");
	if (IS_ERR(class_timer)) {
		ret = PTR_ERR(class_timer);
		printk(KERN_ERR "timer: failed to create sysfs class: %d\n",
		       ret);
		return ret;
	}

	ret = class_create_file(class_timer, &class_attr_inter);
	if (ret) {
		printk(KERN_ERR
		       "timer: failed to create sysfs class attribute inter: %d\n",
		       ret);
		return ret;
	}

	//setup_timer(&inter_timer, inter_callback, 0);

	add_timer(&inter_timer);
	sec = 0;
	mod_timer(&inter_timer, jiffies + msecs_to_jiffies(TIMEOUT));

	printk(KERN_INFO "timer: module loaded\n");
	return 0;
}

static void timer_exit(void)
{
	if (class_timer) {
		class_remove_file(class_timer, &class_attr_inter);
	}
	class_destroy(class_timer);

	del_timer(&inter_timer);

	printk(KERN_INFO "timer: module unloaded\n");
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("timer sysfs");
MODULE_VERSION("0.1");

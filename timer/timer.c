#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>

static ssize_t inter_show(struct class *class, struct class_attribute *attr,
			  char *buffer);

static struct class *class_timer = NULL;
CLASS_ATTR_RO(inter);

static ssize_t inter_show(struct class *class, struct class_attribute *attr,
			  char *buffer)
{
	sprintf(buffer, "time = \n");
	return strlen(buffer);
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

	printk(KERN_INFO "timer: module loaded\n");
	return 0;
}

static void timer_exit(void)
{
	if (class_timer) {
		class_remove_file(class_timer, &class_attr_inter);
	}
	class_destroy(class_timer);
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("timer sysfs");
MODULE_VERSION("0.1");

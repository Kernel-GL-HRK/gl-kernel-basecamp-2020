#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/timer.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

#define TIMEOUT 1000U
#define MAX_HR_BUFF 64
#define DOT_POS 9

static u32 inter_sec = 0;

static ssize_t inter_show(struct class *class, struct class_attribute *attr,
			  char *buffer);
static ssize_t time_show(struct class *class, struct class_attribute *attr,
			 char *buffer);

void inter_callback(struct timer_list *timer);

static struct class *class_timer = NULL;
CLASS_ATTR_RO(inter);
CLASS_ATTR_RO(time);

DEFINE_TIMER(inter_timer, inter_callback);
static struct hrtimer hr_timer;

static ssize_t inter_show(struct class *class, struct class_attribute *attr,
			  char *buffer)
{
	mod_timer(&inter_timer, jiffies + msecs_to_jiffies(TIMEOUT));
	sprintf(buffer, "%u\n", inter_sec);
	inter_sec = 0;
	return strlen(buffer);
}

static ssize_t time_show(struct class *class, struct class_attribute *attr,
			 char *buffer)
{
	ktime_t time = hrtimer_cb_get_time(&hr_timer);
	sprintf(buffer, "%lu\n", time);
	return strlen(buffer);
}

void inter_callback(struct timer_list *timer)
{
	mod_timer(&inter_timer, jiffies + msecs_to_jiffies(TIMEOUT));
	++inter_sec;
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
		printk(KERN_ERR "timer: bad attribute inter create: %d\n", ret);
		return ret;
	}

	ret = class_create_file(class_timer, &class_attr_time);
	if (ret) {
		printk(KERN_ERR "timer: bad attribute time create: %d\n", ret);
		return ret;
	}

	add_timer(&inter_timer);
	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

	mod_timer(&inter_timer, jiffies + msecs_to_jiffies(TIMEOUT));
	hrtimer_start(&hr_timer, KTIME_MAX, HRTIMER_MODE_REL);

	printk(KERN_INFO "timer: module loaded\n");
	return 0;
}

static void timer_exit(void)
{
	if (class_timer) {
		class_remove_file(class_timer, &class_attr_inter);
		class_remove_file(class_timer, &class_attr_time);
	}
	class_destroy(class_timer);

	del_timer(&inter_timer);
	hrtimer_cancel(&hr_timer);

	printk(KERN_INFO "timer: module unloaded\n");
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("timer sysfs");
MODULE_VERSION("0.1");

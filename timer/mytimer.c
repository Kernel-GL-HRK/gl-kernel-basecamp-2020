#include <linux/module.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define CLASS_NAME "mytimer"
#define MSG_LENGTH 128
#define CPU_LOAD_UPDATE 2

static struct timespec64 last_read;
static u64 cpu_load;
static struct timer_list cpu_load_timer;
static struct class *mytimer_class;

static ssize_t show_interval(struct class *class, struct class_attribute *attr, char *buf);

static ssize_t show_time(struct class *class, struct class_attribute *attr, char *buf);

static ssize_t show_load(struct class *class, struct class_attribute *attr, char *buf);

static void cpu_load_callback(struct timer_list *timer);

static struct class_attribute mytimer_interval = __ATTR(interval, 00644, show_interval, NULL);
static struct class_attribute mytimer_time = __ATTR(time, 00644, show_time, NULL);
static struct class_attribute mytimer_load = __ATTR(load, 00644, show_load, NULL);

static ssize_t show_interval(struct class *class, struct class_attribute *attr, char *buffer)
{
	size_t len;
	char buf[MSG_LENGTH];
	struct timespec64 tmp;
	ktime_get_real_ts64(&tmp);
	len = sprintf(buf, "%lld", timespec64_to_ns(&tmp) - timespec64_to_ns(&last_read));
	strncpy(buffer, buf, len);
	last_read = tmp;
	return len;
}

static ssize_t show_time(struct class *class, struct class_attribute *attr, char *buffer)
{
	size_t len;
	char buf[MSG_LENGTH];
	struct timespec64 tmp;
	ktime_get_real_ts64(&tmp);
	len = sprintf(buf, "%lld.%ld", tmp.tv_sec, tmp.tv_nsec);
	strncpy(buffer, buf, len);
	return len;
}

static ssize_t show_load(struct class *class, struct class_attribute *attr, char *buffer)
{
	size_t len;
	char buf[MSG_LENGTH];
	len = sprintf(buf, "%llu%%", cpu_load);
	strncpy(buffer, buf, len);
	return len;
}

static void cpu_load_callback(struct timer_list *timer)
{
	printk(KERN_INFO "Timer: callback\n");
	cpu_load_timer.expires = get_jiffies_64() + (CPU_LOAD_UPDATE * HZ);
	add_timer(&cpu_load_timer);
}

static int __init mytimer_init(void)
{
	int ret;
	pr_info("mytimer: init\n");
	ktime_get_real_ts64(&last_read);
	mytimer_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(mytimer_class))
		return PTR_ERR(mytimer_class);
	
	if ((ret = class_create_file(mytimer_class, &mytimer_interval)) < 0)
		return ret;
	if ((ret = class_create_file(mytimer_class, &mytimer_time)) < 0)
		return ret;
	if ((ret = class_create_file(mytimer_class, &mytimer_load)) < 0)
		return ret;

	/* Timer initialization */
	timer_setup(&cpu_load_timer, cpu_load_callback, 0);
	cpu_load_timer.expires = get_jiffies_64() + (CPU_LOAD_UPDATE * HZ);
	add_timer(&cpu_load_timer);
	return 0;
}

static void __exit mytimer_exit(void)
{
	pr_info("mytimer: exit\n");
	class_remove_file(mytimer_class, &mytimer_interval);
	class_remove_file(mytimer_class, &mytimer_time);
	class_remove_file(mytimer_class, &mytimer_load);
	class_destroy(mytimer_class);
	del_timer(&cpu_load_timer);
}

module_init(mytimer_init);
module_exit(mytimer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrii Revva");
MODULE_DESCRIPTION("Homework #3: Time Management");

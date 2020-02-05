#include <linux/module.h>
#include <linux/time.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define CLASS_NAME "mytimer"
#define MSG_LENGTH 128

static struct timespec64 last_read;
static struct class *mytimer_class;

static ssize_t show_interval(struct class *class, struct class_attribute *attr, char *buf);
static ssize_t store_interval(struct class *class, struct class_attribute *attr, const char *buf, size_t count);

static struct class_attribute mytimer_interval = __ATTR( interval, 00644, show_interval, store_interval);

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

static ssize_t store_interval(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	return count;
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
	return 0;
}

static void __exit mytimer_exit(void)
{
	pr_info("mytimer: exit\n");
	class_remove_file(mytimer_class, &mytimer_interval);
	class_destroy(mytimer_class);
}

module_init(mytimer_init);
module_exit(mytimer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrii Revva");
MODULE_DESCRIPTION("Homework #3: Time Management");

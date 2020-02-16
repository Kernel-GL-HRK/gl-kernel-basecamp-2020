/* SPDX-License-Identifier: GPL-2.0 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/time.h>

static struct timespec64 prev_tspec;

#undef CLASS_ATTR_RO

#define CLASS(name) struct class *class_##name = NULL
#define CLASS_ATTR(name, mode, show, store)                                    \
	struct class_attribute class_attr_##name =                             \
		__ATTR(name, mode, show, store)
#define CLASS_ATTR_RO(name, show) CLASS_ATTR(name, S_IRUGO, show, NULL)

#define CLASS_TIMER "timer"

static ssize_t class_attr_time_show(struct class *class,
				    struct class_attribute *attr, char *buf)
{
	ssize_t ret;
	struct timespec64 tspec;

	ktime_get_real_ts64(&tspec);

	ret = sprintf(buf, "%lld.%ld\n", tspec.tv_sec, tspec.tv_nsec);

	return ret;
}

static ssize_t class_attr_interval_show(struct class *class,
					struct class_attribute *attr, char *buf)
{
	ssize_t ret;
	struct timespec64 tspec;
	s64 tdiff;

	ktime_get_real_ts64(&tspec);

	tdiff = timespec64_to_ns(&tspec) - timespec64_to_ns(&prev_tspec);
	prev_tspec = tspec;

	ret = sprintf(buf, "%lld\n", tdiff);

	return ret;
}

static CLASS(timer);
static CLASS_ATTR_RO(time, &class_attr_time_show);
static CLASS_ATTR_RO(interval, &class_attr_interval_show);

static int __init timer_init(void)
{
	int ret;

	ktime_get_real_ts64(&prev_tspec);

	class_timer = class_create(THIS_MODULE, CLASS_TIMER);
	if (class_timer == NULL) {
		ret = -ENOENT;
		goto err;
	}

	ret = class_create_file(class_timer, &class_attr_time);
	if (ret) {
		goto remove_class_timer;
	}

	ret = class_create_file(class_timer, &class_attr_interval);
	if (ret) {
		goto remove_class_attr_time;
	}

	return 0;

remove_class_attr_time:
	class_remove_file(class_timer, &class_attr_time);
remove_class_timer:
	class_destroy(class_timer);
err:
	return ret;
}

static void __exit timer_exit(void)
{
	class_remove_file(class_timer, &class_attr_interval);
	class_remove_file(class_timer, &class_attr_time);
	class_destroy(class_timer);
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_AUTHOR("Eduard Malokhvii <malohvii.ee@gmail.com>");
MODULE_DESCRIPTION("A solution for timer in kernel space");
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL v2");

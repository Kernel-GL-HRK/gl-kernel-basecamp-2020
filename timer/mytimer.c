// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/list.h>
#include <linux/cpumask.h>
#include <linux/kernel_stat.h>

#define CLASS_NAME "mytimer"
#define MSG_LENGTH 128
#define CPU_LOAD_UPDATE 1

static struct timespec64 last_read;
static u64 cpu_load;
static struct timer_list cpu_load_timer;
static struct class *mytimer_class;
static u64 last_total_load, last_usn_load;

static ssize_t show_interval(struct class *class,
			struct class_attribute *attr,
			char *buf);

static ssize_t show_time(struct class *class,
			struct class_attribute *attr,
			char *buf);

static ssize_t show_load(struct class *class,
			struct class_attribute *attr,
			char *buf);

static void cpu_load_callback(struct timer_list *timer);
static void cpu_load_setup(void);

static const struct class_attribute mytimer_interval =
			__ATTR(interval, 00644, show_interval, NULL);
static const struct class_attribute mytimer_time =
			__ATTR(time, 00644, show_time, NULL);
static const struct class_attribute mytimer_load =
			__ATTR(load, 00644, show_load, NULL);

static ssize_t show_interval(struct class *class,
			struct class_attribute *attr,
			char *buffer)
{
	size_t len;
	char buf[MSG_LENGTH];
	struct timespec64 tmp;

	ktime_get_real_ts64(&tmp);
	len = sprintf(buf, "%lld\n",
	timespec64_to_ns(&tmp) - timespec64_to_ns(&last_read));
	strncpy(buffer, buf, len);
	last_read = tmp;
	return len;
}

static ssize_t show_time(struct class *class,
			struct class_attribute *attr,
			char *buffer)
{
	size_t len;
	char buf[MSG_LENGTH];
	struct timespec64 tmp;

	ktime_get_real_ts64(&tmp);
	len = sprintf(buf, "%lld.%ld\n", tmp.tv_sec, tmp.tv_nsec);
	strncpy(buffer, buf, len);
	return len;
}

static ssize_t show_load(struct class *class,
			struct class_attribute *attr,
			char *buffer)
{
	size_t len;
	char buf[MSG_LENGTH];

	len = sprintf(buf, "%llu%%\n", cpu_load);
	strncpy(buffer, buf, len);
	return len;
}

static inline u64 get_total_cpu_time(u64 *cpustat)
{
	return cpustat[CPUTIME_USER] + cpustat[CPUTIME_NICE]
	+ cpustat[CPUTIME_SYSTEM] + cpustat[CPUTIME_IDLE]
	+ cpustat[CPUTIME_IOWAIT] + cpustat[CPUTIME_IRQ]
	+ cpustat[CPUTIME_SOFTIRQ] + cpustat[CPUTIME_STEAL]
	+ cpustat[CPUTIME_GUEST] + cpustat[CPUTIME_GUEST_NICE];
}

static void cpu_load_callback(struct timer_list *timer)
{
	u64 i = 0;
	u64 user, system, nice, total;

	user = system = nice = total = 0;
	for_each_possible_cpu(i) {
		struct kernel_cpustat kcpustat;
		u64 *cpustat = kcpustat.cpustat;

		kcpustat_cpu_fetch(&kcpustat, i);
		user += cpustat[CPUTIME_USER];
		nice += cpustat[CPUTIME_NICE];
		system += cpustat[CPUTIME_SYSTEM];
		total += get_total_cpu_time(cpustat);
	}
	i = user + system + nice;
	cpu_load = 100 * (i - last_usn_load) / (total - last_total_load);
	printk(KERN_INFO "cpu load: %lld%%\n", cpu_load);
	last_total_load = total;
	last_usn_load = i;

	cpu_load_timer.expires = get_jiffies_64() + (CPU_LOAD_UPDATE * HZ);
	add_timer(&cpu_load_timer);
}

static void cpu_load_setup(void)
{
	int i = 0;

	for_each_possible_cpu(i) {
		struct kernel_cpustat kcpustat;
		u64 *cpustat = kcpustat.cpustat;

		kcpustat_cpu_fetch(&kcpustat, i);
		last_usn_load += cpustat[CPUTIME_USER]
		+ cpustat[CPUTIME_NICE]
		+ cpustat[CPUTIME_SYSTEM];
		last_total_load += get_total_cpu_time(cpustat);
	}
}

static int __init mytimer_init(void)
{
	int ret;

	pr_info("mytimer: init\n");
	ktime_get_real_ts64(&last_read);
	mytimer_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(mytimer_class))
		return PTR_ERR(mytimer_class);
	ret = class_create_file(mytimer_class, &mytimer_interval);
	if (ret < 0)
		return ret;
	ret = class_create_file(mytimer_class, &mytimer_time);
	if (ret < 0)
		return ret;
	ret = class_create_file(mytimer_class, &mytimer_load);
	if (ret < 0)
		return ret;

	timer_setup(&cpu_load_timer, cpu_load_callback, 0);
	cpu_load_timer.expires = get_jiffies_64() + (CPU_LOAD_UPDATE * HZ);
	add_timer(&cpu_load_timer);
	cpu_load_setup();
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

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Andrii Revva");
MODULE_DESCRIPTION("Homework #3: Time Management");

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/hrtimer.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/pci.h>
#include <linux/version.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nick");
MODULE_DESCRIPTION("Timer");
MODULE_VERSION("0.01");

static ssize_t read_sysfs(struct class *class, struct class_attribute *attr,
			  char *buf);
static ssize_t write_sysfs(struct class *class, struct class_attribute *attr,
			   const char *buf, size_t count);

/* sysfs 
 */
static struct class *timer;
static struct class_attribute myTim =
	__ATTR(myTim, S_IWUSR | S_IRUGO, read_sysfs, write_sysfs);


struct st {
	struct hrtimer hrtim;
	ktime_t period;
	enum hrtimer_mode mode;
	unsigned long long nsec;
} myTimer;

static int initTim(void)
{
	myTimer.mode = 0x1;
	myTimer.period = ktime_set(0, 1000);
	hrtimer_init(&myTimer.hrtim, CLOCK_REALTIME, myTimer.mode);
	myTimer.hrtim.function = NULL;
	return 0;
}

static ssize_t read_sysfs(struct class *class, struct class_attribute *attr,
			  char *buf)
{
	return strlen(buf);
}


static ssize_t write_sysfs(struct class *class, struct class_attribute *attr,
			   const char *buf, size_t count)
{
	return count;
}

static int init_sysfs(void)
{
	timer = class_create(THIS_MODULE, "Tim");

	// need to detect bad clacc create

	class_create_file(timer, &myTim);

	return 0;
}

static int exit_sysfs(void)
{
	class_remove_file(timer, &myTim);

	// need to detect bad remove and destroy

	class_destroy(timer);

	return 0;
}


static int __init mod_init(void)
{
    init_sysfs();
	initTim();
	hrtimer_start(&myTimer.hrtim, myTimer.period, myTimer.mode);
	return 0;
}

static void __exit mod_exit(void)
{
    exit_sysfs();
	hrtimer_cancel(&myTimer.hrtim);
}

module_init(mod_init);
module_exit(mod_exit);
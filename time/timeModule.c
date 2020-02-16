#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

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
	return 0;
}

static void __exit mod_exit(void)
{
    exit_sysfs();
}

module_init(mod_init);
module_exit(mod_exit);
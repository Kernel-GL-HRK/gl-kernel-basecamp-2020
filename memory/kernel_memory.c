#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <asm/uaccess.h>

#define BUFF_SIZE 1024

static struct hrtimer hr_timer;
static ktime_t kt1;
static ktime_t kt2;
static ktime_t alloc_time;

static ssize_t kmalloc_show(struct class *class, struct class_attribute *attr,
			    char *buffer);

static struct class *class_alloc;
CLASS_ATTR_RO(kmalloc);

static ssize_t kmalloc_show(struct class *class, struct class_attribute *attr,
			    char *buffer)
{
	void *pdata;

	kt1 = hrtimer_cb_get_time(&hr_timer);
	pdata = kmalloc(BUFF_SIZE, GFP_KERNEL);
	kt2 = hrtimer_cb_get_time(&hr_timer);
	if (!pdata) {
		printk(KERN_WARNING "alloc-test: bad kmalloc: %d\n", ENOMEM);
		return strlen(buffer);
	}
	alloc_time = kt2 - kt1;
	kt1 = hrtimer_cb_get_time(&hr_timer);
	kfree(pdata);
	kt2 = hrtimer_cb_get_time(&hr_timer);
	sprintf(buffer, "%-20llu%-20llu\n", alloc_time, (kt2 - kt1));

	return strlen(buffer);
}

static int __init kernel_memory_init(void)
{
	int ret;

	class_alloc = class_create(THIS_MODULE, "alloc-test");
	if (IS_ERR(class_alloc)) {
		ret = PTR_ERR(class_alloc);
		printk(KERN_ERR "alloc-test: bad create sysfs class: %d\n",
		       ret);
		return ret;
	}

	ret = class_create_file(class_alloc, &class_attr_kmalloc);
	if (ret) {
		printk(KERN_ERR "alloc-test: bad kmalloc attr create: %d\n",
		       ret);
		return ret;
	}

	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hrtimer_start(&hr_timer, KTIME_MAX, HRTIMER_MODE_REL);

	printk(KERN_INFO "alloc-test: module loaded\n");
	return 0;
}

static void __exit kernel_memory_exit(void)
{
	if (class_alloc) {
		class_remove_file(class_alloc, &class_attr_kmalloc);
	}
	class_destroy(class_alloc);

	hrtimer_cancel(&hr_timer);

	printk(KERN_INFO "alloc-test: module unloaded\n");
}

module_init(kernel_memory_init);
module_exit(kernel_memory_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Matvii Zorin");
MODULE_DESCRIPTION("memory allocate/freeing test");
MODULE_VERSION("0.1");

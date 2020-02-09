#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <asm/uaccess.h>

#define BUFF_SIZE 1024

static size_t alloc_size;
static struct hrtimer hr_timer;
static ktime_t kt1;
static ktime_t kt2;
static ktime_t alloc_time;

static ssize_t alloc_size_show(struct class *class,
			       struct class_attribute *attr, char *buffer);
static ssize_t alloc_size_store(struct class *class,
				struct class_attribute *attr,
				const char *buffer, size_t count);
static ssize_t kmalloc_show(struct class *class, struct class_attribute *attr,
			    char *buffer);

static struct class *class_alloc;
CLASS_ATTR_RW(alloc_size);
CLASS_ATTR_RO(kmalloc);

static ssize_t alloc_size_show(struct class *class,
			       struct class_attribute *attr, char *buffer)
{
	sprintf(buffer, "%lu\n", alloc_size);
	return strlen(buffer);
}

static ssize_t alloc_size_store(struct class *class,
				struct class_attribute *attr,
				const char *buffer, size_t count)
{
	sscanf(buffer, "%lu\n", &alloc_size);
	return count;
}

static ssize_t kmalloc_show(struct class *class, struct class_attribute *attr,
			    char *buffer)
{
	void *pdata;

	kt1 = hrtimer_cb_get_time(&hr_timer);
	pdata = kmalloc(alloc_size, GFP_KERNEL);
	kt2 = hrtimer_cb_get_time(&hr_timer);
	if (!pdata) {
		printk(KERN_WARNING "kernel_memory: bad kmalloc: %d\n", ENOMEM);
		return strlen(buffer);
	}
	alloc_time = kt2 - kt1;
	kt1 = hrtimer_cb_get_time(&hr_timer);
	kfree(pdata);
	kt2 = hrtimer_cb_get_time(&hr_timer);
	sprintf(buffer, "%llu %llu\n", alloc_time, (kt2 - kt1));

	return strlen(buffer);
}

static int __init kernel_memory_init(void)
{
	int ret;
	alloc_size = BUFF_SIZE;

	class_alloc = class_create(THIS_MODULE, "alloc_test");
	if (IS_ERR(class_alloc)) {
		ret = PTR_ERR(class_alloc);
		printk(KERN_ERR "kernel_memory: bad create sysfs class: %d\n",
		       ret);
		return ret;
	}

	class_attr_alloc_size.attr.mode = (S_IWUGO | S_IRUGO);
	ret = class_create_file(class_alloc, &class_attr_alloc_size);
	if (ret) {
		printk(KERN_ERR "kernel_memory: bad size attr create: %d\n",
		       ret);
		return ret;
	}

	ret = class_create_file(class_alloc, &class_attr_kmalloc);
	if (ret) {
		printk(KERN_ERR "kernel_memory: bad kmalloc attr create: %d\n",
		       ret);
		return ret;
	}

	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hrtimer_start(&hr_timer, KTIME_MAX, HRTIMER_MODE_REL);

	printk(KERN_INFO "kernel_memory: module loaded\n");
	return 0;
}

static void __exit kernel_memory_exit(void)
{
	if (class_alloc) {
		class_remove_file(class_alloc, &class_attr_alloc_size);
		class_remove_file(class_alloc, &class_attr_kmalloc);
	}
	class_destroy(class_alloc);

	hrtimer_cancel(&hr_timer);

	printk(KERN_INFO "kernel_memory: module unloaded\n");
}

module_init(kernel_memory_init);
module_exit(kernel_memory_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Matvii Zorin");
MODULE_DESCRIPTION("memory allocate/freeing test");
MODULE_VERSION("0.1");

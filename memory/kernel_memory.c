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
static unsigned int page_order;
static struct hrtimer hr_timer;
static ktime_t kt1;
static ktime_t kt2;
static ktime_t alloc_time;

static ssize_t alloc_size_show(struct class *class,
			       struct class_attribute *attr, char *buffer);
static ssize_t alloc_size_store(struct class *class,
				struct class_attribute *attr,
				const char *buffer, size_t count);
static ssize_t page_order_show(struct class *class,
			       struct class_attribute *attr, char *buffer);
static ssize_t page_order_store(struct class *class,
				struct class_attribute *attr,
				const char *buffer, size_t count);
static ssize_t kmalloc_show(struct class *class, struct class_attribute *attr,
			    char *buffer);
static ssize_t vmalloc_show(struct class *class, struct class_attribute *attr,
			    char *buffer);
static ssize_t pages_show(struct class *class, struct class_attribute *attr,
			  char *buffer);

static struct class *class_alloc;
CLASS_ATTR_RW(alloc_size);
CLASS_ATTR_RW(page_order);
CLASS_ATTR_RO(kmalloc);
CLASS_ATTR_RO(vmalloc);
CLASS_ATTR_RO(pages);

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

static ssize_t page_order_show(struct class *class,
			       struct class_attribute *attr, char *buffer)
{
	sprintf(buffer, "%u\n", page_order);
	return strlen(buffer);
}

static ssize_t page_order_store(struct class *class,
				struct class_attribute *attr,
				const char *buffer, size_t count)
{
	sscanf(buffer, "%u\n", &page_order);
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

static ssize_t vmalloc_show(struct class *class, struct class_attribute *attr,
			    char *buffer)
{
	void *pdata;

	kt1 = hrtimer_cb_get_time(&hr_timer);
	pdata = vmalloc(alloc_size);
	kt2 = hrtimer_cb_get_time(&hr_timer);
	if (!pdata) {
		printk(KERN_WARNING "kernel_memory: bad vmalloc: %d\n", ENOMEM);
		return strlen(buffer);
	}
	alloc_time = kt2 - kt1;
	kt1 = hrtimer_cb_get_time(&hr_timer);
	vfree(pdata);
	kt2 = hrtimer_cb_get_time(&hr_timer);
	sprintf(buffer, "%llu %llu\n", alloc_time, (kt2 - kt1));

	return strlen(buffer);
}

static ssize_t pages_show(struct class *class, struct class_attribute *attr,
			  char *buffer)
{
	void *ppages;

	kt1 = hrtimer_cb_get_time(&hr_timer);
	ppages = (void *)__get_free_pages(GFP_KERNEL, page_order);
	kt2 = hrtimer_cb_get_time(&hr_timer);
	if (!ppages) {
		printk(KERN_WARNING "kernel_memory: no free pages: %d\n",
		       ENOMEM);
		return strlen(buffer);
	}
	alloc_time = kt2 - kt1;
	kt1 = hrtimer_cb_get_time(&hr_timer);
	free_pages((unsigned long)ppages, page_order);
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

	class_attr_page_order.attr.mode = (S_IWUGO | S_IRUGO);
	ret = class_create_file(class_alloc, &class_attr_page_order);
	if (ret) {
		printk(KERN_ERR
		       "kernel_memory: bad page order attr create: %d\n",
		       ret);
		return ret;
	}

	ret = class_create_file(class_alloc, &class_attr_kmalloc);
	if (ret) {
		printk(KERN_ERR "kernel_memory: bad kmalloc attr create: %d\n",
		       ret);
		return ret;
	}

	ret = class_create_file(class_alloc, &class_attr_vmalloc);
	if (ret) {
		printk(KERN_ERR "kernel_memory: bad vmalloc attr create: %d\n",
		       ret);
		return ret;
	}

	ret = class_create_file(class_alloc, &class_attr_pages);
	if (ret) {
		printk(KERN_ERR "kernel_memory: bad pages attr create: %d\n",
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
		class_remove_file(class_alloc, &class_attr_page_order);
		class_remove_file(class_alloc, &class_attr_kmalloc);
		class_remove_file(class_alloc, &class_attr_vmalloc);
		class_remove_file(class_alloc, &class_attr_pages);
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

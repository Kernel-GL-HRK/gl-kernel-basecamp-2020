// SPDX-License-Identifier: GPL-2.0 
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/time.h>
#include <linux/device.h>
#include <linux/time64.h>

#define CLASS_NAME "mm_test"
#define TABLE_LENGTH 4096
#define BUF_LENGTH 128
#define TEST1_BYTES 4096UL
#define TEST2_BYTES 65536UL
#define TEST3_BYTES 1048576UL
#define TEST1_PAGES (TEST1_BYTES / PAGE_SIZE)
#define TEST2_PAGES (TEST2_BYTES / PAGE_SIZE)
#define TEST3_PAGES (TEST3_BYTES / PAGE_SIZE)

static char kmalloc_result[TABLE_LENGTH];

static struct class *mm_test_class;

static ssize_t show_result(struct class *class,
			struct class_attribute *attr,
			char *buffer);

static struct class_attribute mm_test_result =
			__ATTR(result, 00644, show_result, NULL);

static ssize_t show_result(struct class *class,
			struct class_attribute *attr,
			char *buffer)
{
	strcpy(buffer, kmalloc_result);
	return strlen(kmalloc_result);
}

static void check_kmalloc(void)
{
	struct timespec64 start, end;
	void *p;
	long alloc_res, free_res;
	char buf[BUF_LENGTH];

	strcat(kmalloc_result, "kmalloc() test:\n");
	strcat(kmalloc_result, "buffer\talloc\tfree\n");
	ktime_get_real_ts64(&start);
	p = kmalloc(TEST1_BYTES, GFP_KERNEL);
	ktime_get_real_ts64(&end);
	start = timespec64_sub(end, start);
	alloc_res = timespec64_to_ns(&start);

	ktime_get_real_ts64(&start);
	kfree(p);
	ktime_get_real_ts64(&end);
	start = timespec64_sub(end, start);
	free_res = timespec64_to_ns(&start);

	sprintf(buf, "%lu\t%ld\t%ld\n", TEST1_BYTES, alloc_res, free_res);
	strcat(kmalloc_result, buf);

	ktime_get_real_ts64(&start);
	p = kmalloc(TEST2_BYTES, GFP_KERNEL);
	ktime_get_real_ts64(&end);
	start = timespec64_sub(end, start);
	alloc_res = timespec64_to_ns(&start);

	ktime_get_real_ts64(&start);
	kfree(p);
	ktime_get_real_ts64(&end);
	start = timespec64_sub(end, start);
	free_res = timespec64_to_ns(&start);

	sprintf(buf, "%lu\t%ld\t%ld\n", TEST2_BYTES, alloc_res, free_res);
	strcat(kmalloc_result, buf);

	ktime_get_real_ts64(&start);
	p = kmalloc(TEST3_BYTES, GFP_KERNEL);
	ktime_get_real_ts64(&end);
	start = timespec64_sub(end, start);
	alloc_res = timespec64_to_ns(&start);

	ktime_get_real_ts64(&start);
	kfree(p);
	ktime_get_real_ts64(&end);
	start = timespec64_sub(end, start);
	free_res = timespec64_to_ns(&start);

	sprintf(buf, "%lu\t%ld\t%ld\n", TEST3_BYTES, alloc_res, free_res);
	strcat(kmalloc_result, buf);
}

static int __init memory_init(void)
{
	int ret;

	mm_test_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(mm_test_class))
		return PTR_ERR(mm_test_class);
	ret = class_create_file(mm_test_class, &mm_test_result);
	if (ret < 0)
		return ret;

	check_kmalloc();
	return 0;
}

static void __exit memory_exit(void)
{
	class_remove_file(mm_test_class, &mm_test_result);
	class_destroy(mm_test_class);
}

module_init(memory_init);
module_exit(memory_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Andrii Revva");
MODULE_DESCRIPTION("Homework #4: Memory managment");

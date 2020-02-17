/* SPDX-License-Identifier: GPL-2.0 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/gfp.h>
#include <linux/time64.h>

#define THIS_MODULE_NAME "memory"
#define THIS_MODULE_TAG THIS_MODULE_NAME ": "

#define TEST_TARGETS 6
#define TEST_MAX_PAGES 128

struct test_data {
	size_t buf_len;
	s64 alloc_duration;
	s64 free_duration;
};

struct test_target {
	char *name;
	void *(*alloc)(size_t, gfp_t);
	void (*free)(const void *);
};

static ssize_t test_memory_alloc(struct test_target *target,
				 struct test_data *data)
{
	void *buf;

	struct timespec64 before;
	struct timespec64 after;
	struct timespec64 diff;

	ktime_get_real_ts64(&before);
	buf = target->alloc(data->buf_len * sizeof(*buf), GFP_KERNEL);
	ktime_get_real_ts64(&after);

	if (buf) {
		diff = timespec64_sub(after, before);
		data->alloc_duration = timespec64_to_ns(&diff);

		ktime_get_real_ts64(&before);
		target->free(buf);
		ktime_get_real_ts64(&after);

		diff = timespec64_sub(after, before);
		data->free_duration = timespec64_to_ns(&diff);
	} else {
		return 1;
	}

	return 0;
}

static void *vmalloc_wrapper(size_t size, gfp_t flags)
{
	return vmalloc((unsigned long)size);
}

static void *vzalloc_wrapper(size_t size, gfp_t flags)
{
	return vzalloc((unsigned long)size);
}

static struct test_target test_targets[TEST_TARGETS] = {
	{ .name = "kmalloc", .alloc = kmalloc, .free = kfree },
	{ .name = "kzalloc", .alloc = kzalloc, .free = kfree },
	{ .name = "kvmalloc", .alloc = kvmalloc, .free = kfree },
	{ .name = "kvzalloc", .alloc = kvzalloc, .free = kfree },
	{ .name = "vmalloc", .alloc = vmalloc_wrapper, .free = vfree },
	{ .name = "vzalloc", .alloc = vzalloc_wrapper, .free = vfree }
};

static int __init memory_init(void)
{
	ssize_t is_out_of_memory;

	struct test_data test_data;
	struct test_target *test_target;

	size_t i;

	for (i = 0; i < TEST_TARGETS; i++) {
		test_target = &test_targets[i];
		test_data.buf_len = 1;

		pr_info(THIS_MODULE_TAG "TEST SUMMARY (%s)\n",
			test_target->name);
		pr_info(THIS_MODULE_TAG "%-20s %-20s %-20s\n", "SIZE (BYTES)",
			"ALLOC (NS)", "FREE (NS)");

		while (test_data.buf_len < PAGE_SIZE * TEST_MAX_PAGES) {
			is_out_of_memory =
				test_memory_alloc(test_target, &test_data);
			if (!is_out_of_memory) {
				pr_info(THIS_MODULE_TAG
					"%-20d %-20lld %-20lld\n",
					test_data.buf_len,
					test_data.alloc_duration,
					test_data.free_duration);
			} else {
				pr_info(THIS_MODULE_TAG "%-20d %-20s %-20s\n",
					test_data.buf_len, "N/A", "N/A");
			}

			test_data.buf_len += PAGE_SIZE - 1;
		}
	}

	return -1;
}

module_init(memory_init);

MODULE_AUTHOR("Eduard Malokhvii <malohvii.ee@gmail.com>");
MODULE_DESCRIPTION("A solution for memory in kernel space");
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL v2");

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/types.h>
#include <linux/hrtimer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nick");
MODULE_DESCRIPTION("Test kernel allocators");
MODULE_VERSION("0.01");


struct timeData
{
    u64 alloc;
    u32 free;
};

static int trykmalloc(u64 size, struct timeData * data)
{
	void *pointer;
	
	data->alloc = hrtimer_cb_get_time(&myTimer.hrtim);
	pointer = kmalloc(size,GFP_KERNEL);
	data->alloc = hrtimer_cb_get_time(&myTimer.hrtim) - data->alloc;

	if (pointer == NULL)
	{
		printk(KERN_INFO "kmalloc failed at %lld bytes\n", size);
		return ERROR; 
	}

	data->free = hrtimer_cb_get_time(&myTimer.hrtim);
	kfree(pointer);
	data->free = hrtimer_cb_get_time(&myTimer.hrtim) - data->free;

	return 0;
}

struct timer_struct {
	struct hrtimer hrtim;
	ktime_t period;
	enum hrtimer_mode mode;
} myTimer;

static int initTim(void)
{
	myTimer.mode = HRTIMER_MODE_REL;
	myTimer.period = KTIME_MAX;
	hrtimer_init(&myTimer.hrtim, CLOCK_MONOTONIC, myTimer.mode);
	//myTimer.hrtim.function = NULL;
	return 0;
}



static int __init mod_init(void)
{
    initTim();
	hrtimer_start(&myTimer.hrtim, myTimer.period, myTimer.mode);
	return 0;
}

static void __exit mod_exit(void)
{
    hrtimer_cancel(&myTimer.hrtim);
}   

module_init(mod_init);
module_exit(mod_exit);
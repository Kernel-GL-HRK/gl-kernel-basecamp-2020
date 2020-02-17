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

static ssize_t vmall_read(struct class *class, struct class_attribute *attr,
                          char *buf);
static ssize_t vmall_write(struct class *class, struct class_attribute *attr,
                           const char *buf, size_t count);

static ssize_t kmall_read(struct class *class, struct class_attribute *attr,
                          char *buf);
static ssize_t kmall_write(struct class *class, struct class_attribute *attr,
                           const char *buf, size_t count);

static ssize_t kzall_read(struct class *class, struct class_attribute *attr,
                          char *buf);
static ssize_t kzall_write(struct class *class, struct class_attribute *attr,
                           const char *buf, size_t count);


static struct class allocdir;

static struct class_attribute kmall =
        __ATTR(kmalloc, S_IWUSR | S_IRUGO, kmall_read, kmall_write);
static struct class_attribute vmall =
        __ATTR(vmalloc, S_IWUSR | S_IRUGO, vmall_read, vmall_write);
static struct class_attribute kzall =
        __ATTR(kzalloc, S_IWUSR | S_IRUGO, kzall__sysfs, kzall__sysfs);

struct timeData {
        u64 alloc;
        u32 free;
};

static u64 size[3];

static int trykmalloc(u64 size, struct timeData *data)
{
        void *pointer;

        data->alloc = hrtimer_cb_get_time(&myTimer.hrtim);
        pointer = kmalloc(size, GFP_KERNEL);
        data->alloc = hrtimer_cb_get_time(&myTimer.hrtim) - data->alloc;

        if (pointer == NULL) {
                printk(KERN_INFO "kmalloc failed at %lld bytes\n", size);
                return ERROR;
        }

        data->free = hrtimer_cb_get_time(&myTimer.hrtim);
        kfree(pointer);
        data->free = hrtimer_cb_get_time(&myTimer.hrtim) - data->free;

        return 0;
}

static int tryvmalloc(u64 size, struct timeData *data)
{
        void *pointer;

        data->alloc = hrtimer_cb_get_time(&myTimer.hrtim);
        pointer = vmalloc(size);
        data->alloc = hrtimer_cb_get_time(&myTimer.hrtim) - data->alloc;

        if (pointer == NULL) {
                printk(KERN_INFO "vmalloc failed at %lld bytes\n", size);
                return ERROR;
        }

        data->free = hrtimer_cb_get_time(&myTimer.hrtim);
        vfree(pointer);
        data->free = hrtimer_cb_get_time(&myTimer.hrtim) - data->free;

        return 0;
}

static int trykzalloc(u64 size, struct timeData *data)
{
        void *pointer;

        data->alloc = hrtimer_cb_get_time(&myTimer.hrtim);
        pointer = kzalloc(size, GFP_KERNEL);
        data->alloc = hrtimer_cb_get_time(&myTimer.hrtim) - data->alloc;

        if (pointer == NULL) {
                printk(KERN_INFO "kzalloc failed at %lld bytes\n", size);
                return ERROR;
        }

        data->free = hrtimer_cb_get_time(&myTimer.hrtim);
        kzfree(pointer);
        data->free = hrtimer_cb_get_time(&myTimer.hrtim) - data->free;

        return 0;
}

static int atoi(const char* str){
    int num = 0;
    int i = 0;
    bool isNegetive = false;
    if(str[i] == '-'){
        isNegetive = true;
        i++;
    }
    while (str[i] && (str[i] >= '0' && str[i] <= '9')){
        num = num * 10 + (str[i] - '0');
        i++;
    }
    if(isNegetive) num = -1 * num;
    return num;
}

static ssize_t vmall_read(struct class *class, struct class_attribute *attr,
                          char *buf)

{
        struct timeData data;

        if (tryvmalloc(size[0], &data)){
                sprintf(buf, "Failed to alloc \n");
        }
        else
        {
                sprintf(buf, "%ld\n%ld\n", data.alloc, data.free);
        }

        return strlen(buf);
}
static ssize_t vmall_write(struct class *class, struct class_attribute *attr,
                           const char *buf, size_t count);
{
		size[0] = atoi(buf);
        return count;
}

static ssize_t kmall_read(struct class *class, struct class_attribute *attr,
                          char *buf)
{
        struct timeData data;

        if (trykmalloc(size[1], &data)){
                sprintf(buf, "Failed to alloc \n");
        }
        else
        {
                sprintf(buf, "%ld\n%ld\n", data.alloc, data.free);
        }

        return strlen(buf);
}
static ssize_t kmall_write(struct class *class, struct class_attribute *attr,
                           const char *buf, size_t count)
{
		size[1] = atoi(buf);
        return count;
}
static ssize_t kzall_read(struct class *class, struct class_attribute *attr,
                          char *buf)
{
        struct timeData data;

        if (trykzalloc(size[2], &data)){
                sprintf(buf, "Failed to alloc \n");
        }
        else
        {
                sprintf(buf, "%ld\n%ld\n", data.alloc, data.free);
        }

        return strlen(buf);
}
static ssize_t kzall_write(struct class *class, struct class_attribute *attr,
                           const char *buf, size_t count)
{
		size[2] = atoi(buf);
        return count;
}


struct timer_struct {
        struct hrtimer hrtim;
        ktime_t period;
        enum hrtimer_mode mode;
} myTimer;

static int init_sysfs(void)
{
        allocdir = class_create(THIS_MODULE, "Alloc");

        class_create_file(allocdir, &kzall);
        class_create_file(allocdir, &vmall);
        class_create_file(allocdir, &kmall);

        return 0;
}

static int exit_sysfs(void)
{
		class_remove_file(allocdir, &vmall);
		class_remove_file(allocdir, &kmall);
		class_remove_file(allocdir, &kzall);

		class_destroy(allocdir);
        return 0;
}

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
        init_sysfs();
        return 0;
}

static void __exit mod_exit(void)
{
        sysfs_exit();
        hrtimer_cancel(&myTimer.hrtim);
}

module_init(mod_init);
module_exit(mod_exit);

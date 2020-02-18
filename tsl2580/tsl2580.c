// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/sysfs.h>
#include <linux/string.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <asm/atomic.h>
#include <linux/mutex.h>
#include <linux/moduleparam.h>
#include <linux/time64.h>
#include <linux/timekeeping.h>

#include "tsl2580_regs.h"

#define TSL2580_CLASS_NAME "tsl2580"

/* Scale factors for lux approximation */
#define LUX_SCALE 16
#define RATIO_SCALE 9
#define ADC_SCALE 16

/* 128x gain scaling factors */
#define ADC0_SCALE_111X 107
#define ADC1_SCALE_111X 115

/* Not sure if the preprocessor performs floating point evaluation */
#define ODFN_K1C 0x009A /* 0.30 * 2 ^ RATIO_SCALE */
#define ODFN_B1C 0x2148 /* 0.130 * 2 ^ LUX_SCALE */
#define ODFN_M1C 0x3D71 /* 0.240 * 2 ^ LUX_SCALE */
#define ODFN_K2C 0x00C3 /* 0.38 * 2 ^ RATIO_SCALE */
#define ODFN_B2C 0x2A37 /* 0.1649 * 2 ^ LUX_SCALE */
#define ODFN_M2C 0x5B30 /* 0.3562 * 2 ^ LUX_SCALE */
#define ODFN_K3C 0x00E6 /* 0.45 * 2 ^ RATIO_SCALE */
#define ODFN_B3C 0x18EF /* 0.0974 * 2 ^ LUX_SCALE */
#define ODFN_M3C 0x2DB9 /* 0.1786 * 2 ^ LUX_SCALE */
#define ODFN_K4C 0x0114 /* 0.54 * 2 ^ RATIO_SCALE */
#define ODFN_B4C 0x0FDF /* 0.062 * 2 ^ LUX_SCALE */
#define ODFN_M4C 0x199A /* 0.10 * 2 ^ LUX_SCALE */
#define ODFN_K5C 0x0114 /* 0.54 * 2 ^ RATIO_SCALE */
#define ODFN_B5C 0x0000 /* 0.00 * 2 ^ LUX_SCALE */
#define ODFN_M5C 0x0000 /* 0.00 * 2 ^ LUX_SCALE */

static u64 read_threashold = HZ;

struct tsl2580_dev {
	struct i2c_client *client;
	u8 dev_id;
	u16 data_adc0;
	u16 data_adc1;
	u16 data_lux;
	struct task_struct *read_data_task;
	wait_queue_head_t data_access_queue;
	atomic_t data_access_state;
	atomic_t thread_stop;
	struct completion data_access_completion;
	struct mutex data_access_mutex;
	struct timespec64 read_ts;
	struct mutex ts_access;
};

static struct tsl2580_dev mydev;
static struct class *tsl2580_class;

static void tsl2580_prepare_read(void);
static s32 tsl2580_read_adc0(struct tsl2580_dev *dev);
static s32 tsl2580_read_adc1(struct tsl2580_dev *dev);
static s32 tsl2580_read_lux(struct tsl2580_dev *dev);
static s32 tsl2580_calc_lux(struct tsl2580_dev *dev, s32 low, s32 high);
static int tsl2580_read_data(struct tsl2580_dev *dev);
static int tsl2580_read_task(void *data);
static int tsl2580_init_sysfs(void);
static ssize_t who_am_i_show(struct class *class,
			struct class_attribute *attr,
			char *buf);
static ssize_t adc0_show(struct class *class,
			struct class_attribute *attr,
			char *buf);
static ssize_t adc1_show(struct class *class,
			struct class_attribute *attr,
			char *buf);
static ssize_t lux_show(struct class *class,
			struct class_attribute *attr,
			char *buf);
static ssize_t rt_show(struct class *class,
			struct class_attribute *attr,
			char *buf);
static ssize_t rt_store(struct class *class,
			struct class_attribute *attr,
			const char *buf,
			size_t size);
static int tsl2580_probe(struct i2c_client *client,
			const struct i2c_device_id *id);
static int tsl2580_remove(struct i2c_client *client);

static const struct i2c_device_id tsl2580_i2c_id[] = {
	{"tsl2580", 0},
	{},
};
MODULE_DEVICE_TABLE(i2c, tsl2580_i2c_id);

static const struct of_device_id tsl2580_of_id[] = {
	{.compatible = "gl,tsl2580"},
	{},
};
MODULE_DEVICE_TABLE(of, tsl2580_of_id);

static struct i2c_driver tsl2580_i2c_driver = {
	.driver = {
		.name = "tsl2580",
		.of_match_table = tsl2580_of_id,
		.owner = THIS_MODULE,
	},
	.probe = tsl2580_probe,
	.remove = tsl2580_remove,
	.id_table = tsl2580_i2c_id,
};

static struct class_attribute class_attr_who_am_i =
				__ATTR(who_am_i, 00644, who_am_i_show, NULL);
static struct class_attribute class_attr_adc0 =
				__ATTR(adc0, 00644, adc0_show, NULL);
static struct class_attribute class_attr_adc1 =
				__ATTR(adc1, 00644, adc1_show, NULL);
static struct class_attribute class_attr_lux =
				__ATTR(lux, 00644, lux_show, NULL);
static struct class_attribute class_attr_rt =
				__ATTR(rt, 00644, rt_show, rt_store);

static s32 tsl2580_read_lux(struct tsl2580_dev *dev)
{
	s32 low, high;
	low = tsl2580_read_adc0(dev);
	if (low < 0)
		return low;
	high = tsl2580_read_adc1(dev);
	if (high < 0)
		return high;
	return tsl2580_calc_lux(dev, low, high);
}

static s32 tsl2580_calc_lux(struct tsl2580_dev *dev, s32 low, s32 high)
{
	u8 gain, ic;
	u32 sc0, sc1, ratio;

	gain = ic = sc0 = sc1 = ratio = 0;

	gain = i2c_smbus_write_byte(dev->client,
	TSL2580_CMD_REG | TSL2580_TRNS_BLOCK | TSL2580_ANALOG_REG);
	if (gain < 0)
		return gain;
	gain = i2c_smbus_read_byte(dev->client);
	if (gain < 0)
		return gain;
	ic = i2c_smbus_write_byte(dev->client,
	TSL2580_CMD_REG | TSL2580_TRNS_BLOCK | TSL2580_TIMING_REG);
	if (ic < 0)
		return ic;
	ic = i2c_smbus_read_byte(dev->client);

	/* Have no idea why it's like this.
	 * Datasheet suggests particularly this formula.
	 */
	if (ic == TSL2580_TIMING_148)
		sc0 = (1 << ADC_SCALE);
	else
		sc0 = (TSL2580_TIMING_148 << ADC_SCALE) / ic;
	
	switch(gain) {
	case TSL2580_ANALOG_GAIN_1X:
		sc1 = sc0;
		break;
	case TSL2580_ANALOG_GAIN_8X:
		sc0 = sc0 >> 3;
		sc1 = sc0;
		break;
	case TSL2580_ANALOG_GAIN_16X:
		sc0 = sc0 >> 4;
		sc1 = sc0;
		break;
	case TSL2580_ANALOG_GAIN_111X:
		sc1 = sc0 / ADC1_SCALE_111X;
		sc0 = sc0 / ADC0_SCALE_111X;
		break;
	}
	low = (sc0 * low) >> ADC_SCALE;
	high = (sc1 * high) >> ADC_SCALE;
	if (low)
		ratio = (high << (RATIO_SCALE + 1)) / low;
	ratio = (ratio + 1) >> 1;
	if (ratio <= ODFN_K1C) {
		sc0 = ODFN_B1C;
		sc1 = ODFN_M1C;
	} else if (ratio <= ODFN_K2C) {
		sc0 = ODFN_B2C;
		sc1 = ODFN_M2C;
	} else if (ratio <= ODFN_K3C) {
		sc0 = ODFN_B3C;
		sc1 = ODFN_M3C;
	} else if (ratio <= ODFN_K4C) {
		sc0 = ODFN_B4C;
		sc1 = ODFN_M4C;
	} else {
		sc0 = ODFN_B5C;
		sc1 = ODFN_M5C;
	}
	low = (low * sc0) - (high * sc1);
	low += (1 << (LUX_SCALE - 1));
	
	return low >> LUX_SCALE;
}

static s32 tsl2580_read_adc1(struct tsl2580_dev *dev)
{
	s32 low, high;

	low = i2c_smbus_write_byte(dev->client,
	TSL2580_CMD_REG | TSL2580_TRNS_BLOCK | TSL2580_DATA1LOW_REG);
	if (low < 0)
		return low;
	low = i2c_smbus_read_byte(dev->client);
	if (low < 0)
		return low;
	high = i2c_smbus_write_byte(dev->client,
	TSL2580_CMD_REG | TSL2580_TRNS_BLOCK | TSL2580_DATA1HIGH_REG);
	if (high < 0)
		return high;
	high = i2c_smbus_read_byte(dev->client);
	if (high < 0)
		return high;
	/* FIXME: assumes little endian */
	return (low | (high << 8));

}

static s32 tsl2580_read_adc0(struct tsl2580_dev *dev)
{
	s32 low, high;

	low = i2c_smbus_write_byte(dev->client,
	TSL2580_CMD_REG | TSL2580_TRNS_BLOCK | TSL2580_DATA0LOW_REG);
	if (low < 0)
		return low;
	low = i2c_smbus_read_byte(dev->client);
	if (low < 0)
		return low;
	high = i2c_smbus_write_byte(dev->client,
	TSL2580_CMD_REG | TSL2580_TRNS_BLOCK | TSL2580_DATA0HIGH_REG);
	if (high < 0)
		return high;
	high = i2c_smbus_read_byte(dev->client);
	if (high < 0)
		return high;
	/* FIXME: assumes little endian */
	return (low | (high << 8));
}

static int tsl2580_read_data(struct tsl2580_dev *dev)
{
	s32 res;

	res = tsl2580_read_adc0(dev);
	if (IS_ERR_VALUE(res))
		return res;
	mydev.data_adc0 = res;
	res = tsl2580_read_adc1(dev);
	if (IS_ERR_VALUE(res))
		return res;
	mydev.data_adc1 = res;
	res = tsl2580_calc_lux(dev, mydev.data_adc0, mydev.data_adc1);
	if (IS_ERR_VALUE(res))
		return res;
	mydev.data_lux = res;
	return 0;
}

static int tsl2580_read_task(void *data)
{
	int ret;

	while (!atomic_read(&mydev.thread_stop)) {
		wait_event(mydev.data_access_queue, atomic_read(&mydev.data_access_state));
		if (atomic_read(&mydev.thread_stop))
			break;
		mutex_lock(&mydev.ts_access);
		ktime_get_real_ts64(&mydev.read_ts);
		mutex_unlock(&mydev.ts_access);

		mutex_lock(&mydev.data_access_mutex);
		ret = tsl2580_read_data(&mydev);
		mutex_unlock(&mydev.data_access_mutex);
		if (IS_ERR_VALUE(ret))
			return ret;

		complete_all(&mydev.data_access_completion);
		atomic_set(&mydev.data_access_state, 0);
	}
	return 0;
}

static int __must_check tsl2580_default_config(struct tsl2580_dev *dev)
{
	int ret;

	if (!dev->client)
		return -EINVAL;
	ret = i2c_smbus_write_byte(dev->client,
	TSL2580_CMD_REG| TSL2580_TRNS_BLOCK | TSL2580_CTRL_REG);
	if (IS_ERR_VALUE(ret))
		return ret;
	/* Enable TSL2580 */
	ret = i2c_smbus_write_byte(dev->client,
	TSL2580_CTRL_POWER | TSL2580_CTRL_ADC_EN);
	if (IS_ERR_VALUE(ret))
		return ret;
	ret = i2c_smbus_write_byte(dev->client,
	TSL2580_CMD_REG | TSL2580_TRNS_BLOCK | TSL2580_TIMING_REG);
	if (IS_ERR_VALUE(ret))
		return ret;
	/* 148 integration cycles by default */
	ret = i2c_smbus_write_byte(dev->client, TSL2580_TIMING_148);
	if (IS_ERR_VALUE(ret))
		return ret;
	ret = i2c_smbus_write_byte(dev->client,
	TSL2580_CMD_REG | TSL2580_TRNS_BLOCK | TSL2580_ANALOG_REG);
	if (IS_ERR_VALUE(ret))
		return ret;
	/* 16x analog gain by default */
	ret = i2c_smbus_write_byte(dev->client, TSL2580_ANALOG_GAIN_16X);
	if (IS_ERR_VALUE(ret))
		return ret;

	return 0;
}

static int tsl2580_init_sysfs(void)
{
	int ret;

	tsl2580_class = class_create(THIS_MODULE, TSL2580_CLASS_NAME);
	if (IS_ERR(tsl2580_class)) {
		ret = PTR_ERR(tsl2580_class);
		pr_err("tsl2580: failed to create a class; error %d\n", ret);
		goto err;
	}
	ret = class_create_file(tsl2580_class, &class_attr_who_am_i);
	if (IS_ERR_VALUE(ret)) {
		pr_err("tsl2580: error %d creating a who_am_i attribute\n", ret);
		goto err;
	}
	ret = class_create_file(tsl2580_class, &class_attr_adc0);
	if (IS_ERR_VALUE(ret)) {
		pr_err("tsl2580: error %d creating an adc0 attribute\n", ret);
		goto err;
	}
	ret = class_create_file(tsl2580_class, &class_attr_adc1);
	if (IS_ERR_VALUE(ret)) {
		pr_err("tsl2580: error %d creating an adc1 attribute\n", ret);
		goto err;
	}
	ret = class_create_file(tsl2580_class, &class_attr_lux);
	if (IS_ERR_VALUE(ret)) {
		pr_err("tsl2580: error %d creating a lux attribute\n", ret);
		goto err;
	}
	ret = class_create_file(tsl2580_class, &class_attr_rt);
	if (IS_ERR_VALUE(ret)) {
		pr_err("tsl2580: error %d creating rt attribute\n", ret);
		goto err;
	}

	return 0;
err:
	class_remove_file(tsl2580_class, &class_attr_who_am_i);
	class_remove_file(tsl2580_class, &class_attr_adc0);
	class_remove_file(tsl2580_class, &class_attr_adc1);
	class_remove_file(tsl2580_class, &class_attr_lux);
	class_destroy(tsl2580_class);
	return ret;
}

static int tsl2580_probe(struct i2c_client *client,
			const struct i2c_device_id *device_id)
{
	int ret;
	u8 id;

	pr_info("tsl2580: i2c client address is 0x%X\n", client->addr);

	mydev.client = client;
	ret = i2c_smbus_write_byte(mydev.client,
	TSL2580_CMD_REG | TSL2580_TRNS_BLOCK | TSL2580_ID_REG);
	if (IS_ERR_VALUE(ret)) {
		pr_err("tsl2580: error %d writing to the device\n", ret);
		return ret;
	}
	id = i2c_smbus_read_byte(mydev.client);
	id &= 0xF0;
	if (id != TSL2580_LOW_ID && id != TSL2581_LOW_ID) {
		pr_warn("tsl2580: wrong device id\n");
		return -EINVAL;
	}
	mydev.dev_id = id;
	ret = tsl2580_default_config(&mydev);
	if (IS_ERR_VALUE(ret)) {
		pr_err("tsl2580: failed to configure the device\n");
		return ret;
	}
	ret = tsl2580_init_sysfs();
	if (IS_ERR_VALUE(ret)) {
		pr_err("tsl2580: error %d initializing sysfs\n", ret);
		return ret;
	}
	mutex_init(&mydev.data_access_mutex);
	mutex_init(&mydev.ts_access);
	init_completion(&mydev.data_access_completion);
	init_waitqueue_head(&mydev.data_access_queue);
	atomic_set(&mydev.data_access_state, 0);
	atomic_set(&mydev.thread_stop, 0);
	mydev.read_data_task = kthread_run(tsl2580_read_task, NULL, "tsl2580_read_thread");
	read_threashold = jiffies_to_nsecs(read_threashold);
	return 0;
}

static int tsl2580_remove(struct i2c_client *client)
{
	pr_info("tsl2580: remove start\n");
	if (mydev.read_data_task) {
		atomic_set(&mydev.thread_stop, 1);
		atomic_set(&mydev.data_access_state, 1);
		wake_up(&mydev.data_access_queue);
	}
	pr_info("tsl2580: remove end\n");

	return 0;
}


static ssize_t who_am_i_show(struct class *class,
			struct class_attribute *attr,
			char *buf)
{
	char *type;
	
	if (mydev.dev_id == TSL2580_LOW_ID)
		type = "2580\n";
	else if (mydev.dev_id == TSL2581_LOW_ID)
		type = "2581\n";
	else
		type = "Unknow device type\n";

	return sprintf(buf, type);
}

static void tsl2580_prepare_read(void)
{
	struct timespec64 ts;
	u64 tmp;
	
	mutex_lock(&mydev.ts_access);
	tmp = timespec64_to_ns(&mydev.read_ts);
	mutex_unlock(&mydev.ts_access);

	ktime_get_real_ts64(&ts);
	if (timespec64_to_ns(&ts) > (tmp + read_threashold)) {
		if (atomic_read(&mydev.data_access_state) == 0) {
			atomic_set(&mydev.data_access_state, 1);
			wake_up(&mydev.data_access_queue);
		}
		wait_for_completion(&mydev.data_access_completion);
	}
}

static ssize_t adc0_show(struct class *class,
			struct class_attribute *attr,
			char *buf)
{
	tsl2580_prepare_read();
	return sprintf(buf, "%d\n", mydev.data_adc0);

}

static ssize_t adc1_show(struct class *class,
			struct class_attribute *attr,
			char *buf)
{
	tsl2580_prepare_read();
	return sprintf(buf, "%d\n", mydev.data_adc1);
}

static ssize_t lux_show(struct class *class,
			struct class_attribute *attr,
			char *buf)
{
	tsl2580_prepare_read();
	return sprintf(buf, "%d\n", mydev.data_lux);
}

static ssize_t rt_show(struct class *class,
		struct class_attribute *attr,
		char *buf)
{
	return sprintf(buf, "%llu\n", nsecs_to_jiffies64(read_threashold));
}

static ssize_t rt_store(struct class *class,
			struct class_attribute *attr,
			const char *buf,
			size_t size)
{
	sscanf(buf, "%llu", &read_threashold);
	read_threashold = jiffies_to_nsecs(read_threashold);
	return size;
}

static int __init tsl2580_init(void)
{
	int ret;

	ret = i2c_add_driver(&tsl2580_i2c_driver);
	if (IS_ERR_VALUE(ret)) {
		pr_err("tsl2580: error %d adding an i2c driver\n", ret);
		return ret;
	}
	pr_info("tsl2580: i2c driver created\n");

	pr_info("tsl2580: init succeded\n");

	return 0;
}

static void __exit tsl2580_exit(void)
{
	pr_info("tsl2580: enter exit\n");
	class_remove_file(tsl2580_class, &class_attr_who_am_i);
	class_remove_file(tsl2580_class, &class_attr_adc0);
	class_remove_file(tsl2580_class, &class_attr_adc1);
	class_remove_file(tsl2580_class, &class_attr_lux);
	class_remove_file(tsl2580_class, &class_attr_rt);
	class_destroy(tsl2580_class);
	i2c_del_driver(&tsl2580_i2c_driver);
	pr_info("tsl2580: exit\n");
}

module_init(tsl2580_init);
module_exit(tsl2580_exit);

MODULE_AUTHOR("Andrii Revva <milaner.work@gmail.com>");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("TSL2580 driver");

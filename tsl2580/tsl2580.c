// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/sysfs.h>
#include <linux/string.h>

#include "tsl2580_regs.h"

#define TSL2580_CLASS_NAME "tsl2580"

struct tsl2580_dev {
	struct i2c_client *client;
};

static struct tsl2580_dev mydev;
static struct class *tsl2580_class;

static ssize_t who_am_i_show(struct class *class,
			struct class_attribute *attr,
			char *buf);
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

static int __must_check tsl2580_default_config(struct tsl2580_dev *dev)
{
	int ret;

	if (!dev->client)
		return -EINVAL;
	ret = i2c_smbus_write_byte(dev->client,
	TSL2580_CMD_REG| TSL2580_TRNS_BLOCK | TSL2580_CTRL_REG);
	if (ret < 0)
		return ret;
	/* Enable TSL2580 */
	ret = i2c_smbus_write_byte(dev->client,
	TSL2580_CTRL_POWER | TSL2580_CTRL_ADC_EN);
	if (ret < 0)
		return ret;
	ret = i2c_smbus_write_byte(dev->client,
	TSL2580_CMD_REG | TSL2580_TRNS_BLOCK | TSL2580_TIMING_REG);
	if (ret < 0)
		return ret;
	/* 148 integration cycles by default */
	ret = i2c_smbus_write_byte(dev->client, TSL2580_TIMING_148);
	if (ret < 0)
		return ret;
	ret = i2c_smbus_write_byte(dev->client,
	TSL2580_CMD_REG | TSL2580_TRNS_BLOCK | TSL2580_ANALOG_REG);
	if (ret < 0)
		return ret;
	/* 16x analog gain by default */
	ret = i2c_smbus_write_byte(dev->client, TSL2580_ANALOG_GAIN_16X);
	if (ret < 0)
		return ret;

	return 0;
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
	if (ret < 0) {
		pr_err("tsl2580: error %d writing to the device\n", ret);
		return ret;
	}
	id = i2c_smbus_read_byte(mydev.client);
	id &= 0xF0;
	if (id != TSL2580_LOW_ID && id != TSL2581_LOW_ID) {
		pr_warn("tsl2580: wrong device id\n");
		return -EINVAL;
	}
	ret = tsl2580_default_config(&mydev);
	if (ret < 0) {
		pr_err("tsl2580: failed to configure the device\n");
		return ret;
	}
	return 0;
}

static int tsl2580_remove(struct i2c_client *client)
{
	pr_info("tsl2580: remove\n");
	return 0;
}


static ssize_t who_am_i_show(struct class *class,
			struct class_attribute *attr,
			char *buf)
{
	char *type;
	u8 id;
	s32 ret;

	ret = i2c_smbus_write_byte(mydev.client,
	TSL2580_CMD_REG | TSL2580_TRNS_BLOCK | TSL2580_ID_REG);
	if (ret < 0) {
		pr_err("tsl2580: error %d writing to the device\n", ret);
		return ret;
	}
	
	id = i2c_smbus_read_byte(mydev.client);
	id &= 0xF0;
	if (id == TSL2580_LOW_ID)
		type = "2580\n";
	else if (id == TSL2581_LOW_ID)
		type = "2581\n";
	else
		type = "Unknow device type\n";
	ret = sprintf(buf, type);

	return ret;
}

static int __init tsl2580_init(void)
{
	int ret;

	ret = i2c_add_driver(&tsl2580_i2c_driver);
	if (ret < 0) {
		pr_err("tsl2580: error %d adding an i2c driver\n", ret);
		return ret;
	}
	pr_info("tsl2580: i2c driver created\n");

	tsl2580_class = class_create(THIS_MODULE, TSL2580_CLASS_NAME);
	if (IS_ERR(tsl2580_class)) {
		ret = PTR_ERR(tsl2580_class);
		pr_err("tsl2580: failed to create a class; error %ld\n",
		PTR_ERR(tsl2580_class));
		goto err;
	}
	
	ret = class_create_file(tsl2580_class, &class_attr_who_am_i);
	if (ret < 0) {
		pr_err("tsl2580: error %d creating a who_am_i attribute\n", ret);
		goto err;
	}

	pr_info("tsl2580: init succeded\n");

	return 0;
err:
	class_remove_file(tsl2580_class, &class_attr_who_am_i);
	class_destroy(tsl2580_class);
	return ret;
}

static void __exit tsl2580_exit(void)
{
	pr_info("tsl2580: enter exit\n");
	class_remove_file(tsl2580_class, &class_attr_who_am_i);
	class_destroy(tsl2580_class);
	i2c_del_driver(&tsl2580_i2c_driver);
	pr_info("tsl2580: exit\n");
}

module_init(tsl2580_init);
module_exit(tsl2580_exit);

MODULE_AUTHOR("Andrii Revva <milaner.work@gmail.com>");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("TSL2580 driver");

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "mpu6050-regs.h"

#define PRINT_READ_RES

struct mpu6050_data {
	struct i2c_client *client;
	s16 accel_values[3];
	s16 gyro_values[3];
	s16 temperature;
};

static struct mpu6050_data g_mpu6050_data;

static int mpu6050_read_data(void)
{
	s16 temp;

	if (g_mpu6050_data.client == 0) {
		pr_err("mpu6050: bad i2c client initialize\n");
		return -ENODEV;
	}

	/* accel */
	g_mpu6050_data.accel_values[0] = (s16)((u16)i2c_smbus_read_word_swapped(
		g_mpu6050_data.client, REG_ACCEL_XOUT_H));
	g_mpu6050_data.accel_values[1] = (s16)((u16)i2c_smbus_read_word_swapped(
		g_mpu6050_data.client, REG_ACCEL_YOUT_H));
	g_mpu6050_data.accel_values[2] = (s16)((u16)i2c_smbus_read_word_swapped(
		g_mpu6050_data.client, REG_ACCEL_ZOUT_H));
	/* gyro */
	g_mpu6050_data.gyro_values[0] = (s16)((u16)i2c_smbus_read_word_swapped(
		g_mpu6050_data.client, REG_GYRO_XOUT_H));
	g_mpu6050_data.gyro_values[1] = (s16)((u16)i2c_smbus_read_word_swapped(
		g_mpu6050_data.client, REG_GYRO_YOUT_H));
	g_mpu6050_data.gyro_values[2] = (s16)((u16)i2c_smbus_read_word_swapped(
		g_mpu6050_data.client, REG_GYRO_ZOUT_H));
	/* temp */
	/* Temperature in degrees C = (TEMP_OUT value)/340 + 36.53 */
	temp = (s16)((u16)i2c_smbus_read_word_swapped(g_mpu6050_data.client,
						      REG_TEMP_OUT_H));
	g_mpu6050_data.temperature = (temp + 12420 + 170) / 340;

#ifdef PRINT_READ_RES
	pr_info("mpu6050: sensor data read:\n");
	pr_info("mpu6050: ACCEL[X,Y,Z] = [%d, %d, %d]\n",
		g_mpu6050_data.accel_values[0], g_mpu6050_data.accel_values[1],
		g_mpu6050_data.accel_values[2]);
	pr_info("mpu6050: GYRO[X,Y,Z] = [%d, %d, %d]\n",
		g_mpu6050_data.gyro_values[0], g_mpu6050_data.gyro_values[1],
		g_mpu6050_data.gyro_values[2]);
	pr_info("mpu6050: TEMP = %d\n", g_mpu6050_data.temperature);
#endif

	return 0;
}

static int mpu6050_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	int ret;

	pr_info("mpu6050: i2c client address is 0x%X\n", client->addr);

	/* Read who_am_i register */
	ret = i2c_smbus_read_byte_data(client, REG_WHO_AM_I);
	if (IS_ERR_VALUE(ret)) {
		pr_err("mpu6050: i2c_smbus_read_byte_data() failed with error: %d\n",
		       ret);
		return ret;
	}
	if (ret != MPU6050_WHO_AM_I) {
		pr_err("mpu6050: wrong i2c device found: expected 0x%X, found 0x%X\n",
		       MPU6050_WHO_AM_I, ret);
		return -1;
	}
	pr_info("mpu6050: i2c mpu6050 device found, WHO_AM_I register value = 0x%X\n",
		ret);

	g_mpu6050_data.client = client;

	/* Setup the device */
	/* No error handling here! */
	i2c_smbus_write_byte_data(client, REG_CONFIG, 0);
	i2c_smbus_write_byte_data(client, REG_GYRO_CONFIG, 0);
	i2c_smbus_write_byte_data(client, REG_ACCEL_CONFIG, 0);
	i2c_smbus_write_byte_data(client, REG_FIFO_EN, 0);
	i2c_smbus_write_byte_data(client, REG_INT_PIN_CFG, 0);
	i2c_smbus_write_byte_data(client, REG_INT_ENABLE, 0);
	i2c_smbus_write_byte_data(client, REG_USER_CTRL, 0);
	i2c_smbus_write_byte_data(client, REG_PWR_MGMT_1, 0);
	i2c_smbus_write_byte_data(client, REG_PWR_MGMT_2, 0);

	pr_info("mpu6050: i2c driver probed\n");
	return 0;
}

static int mpu6050_remove(struct i2c_client *client)
{
	g_mpu6050_data.client = 0;

	pr_info("mpu6050: i2c driver removed\n");
	return 0;
}

static const struct of_device_id mpu6050_of_dev_id[] = {
	{
		.name = "gl_mpu6050",
		.compatible = "gl,mpu6050",
	},
	{}
};
MODULE_DEVICE_TABLE(of, mpu6050_of_dev_id);

static const struct i2c_device_id mpu6050_i2c_dev_id[] = { { "mpu6050", 0 },
							   {} };
MODULE_DEVICE_TABLE(i2c, mpu6050_i2c_dev_id);

static struct i2c_driver mpu6050_driver = {
	.driver = {
		.name = "mpu6050",
		.of_match_table = of_match_ptr(mpu6050_of_dev_id),
	},
	.probe = mpu6050_probe,
	.remove = mpu6050_remove,
	.id_table = mpu6050_i2c_dev_id,
};

static ssize_t accel_x_show(struct class *class, struct class_attribute *attr,
			    char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[0]);
	return strlen(buf);
}

static ssize_t accel_y_show(struct class *class, struct class_attribute *attr,
			    char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[1]);
	return strlen(buf);
}

static ssize_t accel_z_show(struct class *class, struct class_attribute *attr,
			    char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[2]);
	return strlen(buf);
}

static ssize_t gyro_x_show(struct class *class, struct class_attribute *attr,
			   char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[0]);
	return strlen(buf);
}

static ssize_t gyro_y_show(struct class *class, struct class_attribute *attr,
			   char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[1]);
	return strlen(buf);
}

static ssize_t gyro_z_show(struct class *class, struct class_attribute *attr,
			   char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[2]);
	return strlen(buf);
}

static ssize_t temperature_show(struct class *class,
				struct class_attribute *attr, char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", g_mpu6050_data.temperature);
	return strlen(buf);
}

CLASS_ATTR_RO(accel_x);
CLASS_ATTR_RO(accel_y);
CLASS_ATTR_RO(accel_z);
CLASS_ATTR_RO(gyro_x);
CLASS_ATTR_RO(gyro_y);
CLASS_ATTR_RO(gyro_z);
CLASS_ATTR_RO(temperature);

static struct class *mpu6050_class;

static int __init mpu6050_init(void)
{
	int ret;

	ret = i2c_add_driver(&mpu6050_driver);
	if (ret) {
		pr_err("mpu6050: failed to add i2c driver: %d\n", ret);
		return ret;
	}

	mpu6050_class = class_create(THIS_MODULE, "mpu6050");
	if (IS_ERR(mpu6050_class)) {
		ret = PTR_ERR(mpu6050_class);
		pr_err("mpu6050: bad create sysfs class: %d\n", ret);
		return ret;
	}

	ret = class_create_file(mpu6050_class, &class_attr_accel_x);
	if (ret) {
		pr_err("mpu6050: bad accel_x attr create: %d\n", ret);
		return ret;
	}

	ret = class_create_file(mpu6050_class, &class_attr_accel_y);
	if (ret) {
		pr_err("mpu6050: bad accel_y attr create: %d\n", ret);
		return ret;
	}

	ret = class_create_file(mpu6050_class, &class_attr_accel_z);
	if (ret) {
		pr_err("mpu6050: bad accel_z attr create: %d\n", ret);
		return ret;
	}

	ret = class_create_file(mpu6050_class, &class_attr_gyro_x);
	if (ret) {
		pr_err("mpu6050: bad gyro_x attr create: %d\n", ret);
		return ret;
	}

	ret = class_create_file(mpu6050_class, &class_attr_gyro_y);
	if (ret) {
		pr_err("mpu6050: bad gyro_y attr create: %d\n", ret);
		return ret;
	}

	ret = class_create_file(mpu6050_class, &class_attr_gyro_z);
	if (ret) {
		pr_err("mpu6050: bad gyro_z attr create: %d\n", ret);
		return ret;
	}

	ret = class_create_file(mpu6050_class, &class_attr_temperature);
	if (ret) {
		pr_err("mpu6050: bad temperature attr create: %d\n", ret);
		return ret;
	}

	pr_info("mpu6050: module loaded\n");
	return 0;
}

static void __exit mpu6050_exit(void)
{
	if (mpu6050_class) {
		class_remove_file(mpu6050_class, &class_attr_accel_x);
		class_remove_file(mpu6050_class, &class_attr_accel_y);
		class_remove_file(mpu6050_class, &class_attr_accel_z);
		class_remove_file(mpu6050_class, &class_attr_gyro_x);
		class_remove_file(mpu6050_class, &class_attr_gyro_y);
		class_remove_file(mpu6050_class, &class_attr_gyro_z);
		class_remove_file(mpu6050_class, &class_attr_temperature);

		class_destroy(mpu6050_class);
	}

	i2c_del_driver(&mpu6050_driver);

	pr_info("mpu6050: module unloaded\n");
}

module_init(mpu6050_init);
module_exit(mpu6050_exit);

MODULE_DESCRIPTION("mpu6050 I2C acc&gyro");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

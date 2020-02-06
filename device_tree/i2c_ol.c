#include <linux/module.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/mod_devicetable.h>
#include <linux/i2c.h>

static int world_misc_open(struct inode *inode, struct file *file)
{
	pr_info("world_misc_open() is called.\n");
	return 0;
}

static int world_misc_close(struct inode *inode, struct file *file)
{
	pr_info("world_misc_close() is called.\n");
	return 0;
}

static long world_misc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	pr_info("world_misc_ioctl() is called. cmd = %d, arg = %ld\n", cmd, arg);
	return 0;
}

static const struct file_operations world_misc_fops = {
	.owner = THIS_MODULE,
	.open = world_misc_open,
	.release = world_misc_close,
	.unlocked_ioctl = world_misc_ioctl,
};

static struct miscdevice world_miscdev = {
		.minor = MISC_DYNAMIC_MINOR,
		.name = "world",
		.fops = &world_misc_fops,
};

static int world_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret_val;
	pr_info("world_probe() function is called.\n");
	ret_val = misc_register(&world_miscdev);

	if (ret_val != 0) {
		pr_err("could not register the misc device world");
		return ret_val;
	}

	pr_info("world: got minor %i\n",world_miscdev.minor);
	return 0;
}

static int world_remove(struct i2c_client *client)
{
	pr_info("world_remove() function is called.\n");
	misc_deregister(&world_miscdev);
	return 0;
}

static const struct of_device_id my_of_ids[] = {
	{ .compatible = "hello,world"},
	{},
};
MODULE_DEVICE_TABLE(of, my_of_ids);

static const struct i2c_device_id my_i2c_ids[] = {
	{ "world", 0 },
	{},
};
MODULE_DEVICE_TABLE(i2c, my_i2c_ids);

static struct i2c_driver my_i2c_driver = {
	.driver = {
		.name = "i2c_ol",
		.of_match_table = my_of_ids,
		.owner = THIS_MODULE,
	},
	.id_table = my_i2c_ids,
	.probe = world_probe,
	.remove = world_remove,
};

static int __init i2c_world_init(void)
{
	pr_info("i2c world driver: init\n");
	i2c_add_driver(&my_i2c_driver);
	return 0;
}

static void __exit i2c_world_exit(void)
{
	pr_info("i2c world driver: exit\n");
	i2c_del_driver(&my_i2c_driver);
}

module_init(i2c_world_init);
module_exit(i2c_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrii Revva");
MODULE_DESCRIPTION("i2c device tree driver");

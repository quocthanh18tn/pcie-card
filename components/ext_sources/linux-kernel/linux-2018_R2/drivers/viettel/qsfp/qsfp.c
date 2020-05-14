#include <linux/module.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/spi/spi.h>
#include <linux/i2c.h>
#include <linux/param.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/string.h>
#include <linux/bitrev.h>
#include <linux/of_device.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>
#include <linux/iio/events.h>
#include <linux/iio/buffer.h>
#include <linux/iio/kfifo_buf.h>
#include <linux/iio/trigger.h>
#include <linux/iio/trigger_consumer.h>
#include <linux/iio/triggered_buffer.h>
#include <linux/regmap.h>
#include <linux/timer.h>
#include "qsfp.h"
#include "../rrudef.h"

static int qsfp_read(struct i2c_client *client, unsigned addr){
	int ret;
	ret = i2c_smbus_read_byte_data(client,addr);
	return ret;
}
static int qsfp_write(struct i2c_client *client, unsigned addr, unsigned value){
	int ret;
	ret = i2c_smbus_write_byte_data(client,addr,value);
	return ret;
}

static int qsfp_reg_access(struct iio_dev *indio_dev, unsigned reg, unsigned writeval, unsigned *readval){
	int ret;
	struct qsfp_state *st = iio_priv(indio_dev);
	if (readval == NULL) {
		//write value to registers
		mutex_lock(&indio_dev->mlock);
		ret = qsfp_write(st->client, reg, writeval);
		mutex_unlock(&indio_dev->mlock);
		return ret;
	}
	else{
		//read value from registers
		mutex_lock(&indio_dev->mlock);
		*readval = qsfp_read(st->client, reg);
		mutex_unlock(&indio_dev->mlock);
		if (*readval < 0) return *readval;
		else return 0;
	}
}

static const struct iio_info qsfp_info = {
	.debugfs_reg_access = &qsfp_reg_access,
	.driver_module = THIS_MODULE,
};

static int qsfp_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	pr_info("thanhpq7 QSPF\n");
	int ret;
	struct iio_dev *indio_dev;
	struct qsfp_state *st;
	//pr_notice("Probing device %s\n", client->dev.of_node->name);
	if (client->addr == SFP_DIAGNOSTICS){
		pr_notice("Probing sfp diagnostic with name %s\n", client->dev.of_node->name);
	}
	if (client->addr == SFP_VENDOR_INFO){
		pr_notice("Probing sfp vendor with name %s\n", client->dev.of_node->name);
	}
	/* Allocate IIO device */
	indio_dev = devm_iio_device_alloc(&client->dev, sizeof(*st));
	if (!indio_dev) {
	 	dev_err(&client->dev, "IIO allocation failed!\n");
	 	return -ENOMEM;
	}
	st = iio_priv(indio_dev);
	i2c_set_clientdata(client, indio_dev);
	st->client = client;
	if (client->addr == SFP_DIAGNOSTICS){
		st->type = TYPE_DIAGNOSTIC;
	}
	if (client->addr == SFP_VENDOR_INFO){
		st->type = TYPE_VENDOR;
	}

	indio_dev->dev.parent = &client->dev;
	indio_dev->name = client->dev.of_node->name;
	indio_dev->info = &qsfp_info;
	indio_dev->modes = INDIO_DIRECT_MODE;
	pr_notice("Start qsfp IIO device registration ... ");
	ret = iio_device_register(indio_dev);
	if(ret==0){
		pr_notice("done\n");
	}
	if(ret<0){
		pr_notice("failed\n");
		return -ENOMEM;
	}
	return 0;
}

static int qsfp_remove(struct i2c_client *client)
{	
	struct iio_dev *indio_dev = i2c_get_clientdata(client);
	struct qsfp_state *st = iio_priv(indio_dev);
	kfree(st);
	iio_device_unregister(indio_dev);
	kfree(client);
	return 0;
}

static const struct i2c_device_id qsfp_id[] = {
    {"sfp_1_vendor", RRU_ID_SFP_1_VENDOR},
    {"sfp_1_diagnostic", RRU_ID_SFP_1_DIAGNOSTIC},
    {"sfp_2_vendor", RRU_ID_SFP_2_VENDOR},
    {"sfp_2_diagnostic", RRU_ID_SFP_2_DIAGNOSTIC},
    {"sfp_3_vendor", RRU_ID_SFP_3_VENDOR},
    {"sfp_3_diagnostic", RRU_ID_SFP_3_DIAGNOSTIC},
    {"sfp_4_vendor", RRU_ID_SFP_4_VENDOR},
    {"sfp_4_diagnostic", RRU_ID_SFP_4_DIAGNOSTIC},
    {},
};

MODULE_DEVICE_TABLE(i2c, qsfp_id);


static struct i2c_driver qsfp_driver = {
	.driver = {
		.name = "qsfp",
		.owner = THIS_MODULE,
		},
	.probe = qsfp_probe,
	.remove = qsfp_remove,
    .id_table = qsfp_id,
};

module_i2c_driver(qsfp_driver);

MODULE_AUTHOR("CUONG PHAM");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("qsfp DRIVER");


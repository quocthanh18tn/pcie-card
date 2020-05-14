#include <linux/module.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/spi/spi.h>
#include <linux/i2c.h>
#include <linux/param.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_address.h>
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
#include <linux/hrtimer.h>
#include <linux/delay.h>
#include "fakeiio.h"
#include "../rrudef.h"

static int fakeiio_reg_access(struct iio_dev *indio_dev, unsigned reg, unsigned writeval, unsigned *readval){
	int ret;
	struct fakeiio_state *st = iio_priv(indio_dev);
	if (readval == NULL) {
		//write value to registers
		if(reg<100){
			mutex_lock(&indio_dev->mlock);
			st->fake_regs[reg] = writeval;
			mutex_unlock(&indio_dev->mlock);
			return 0;
		}
		else{
			return -1;
		}
	}
	else{
		//read value from registers
		if(reg<100){
			mutex_lock(&indio_dev->mlock);
			*readval = st->fake_regs[reg];
			mutex_unlock(&indio_dev->mlock);
			return 0;
		}
		else{
			return -1;
		}
	}
}

static int fakeiio_read_raw_iio(struct iio_dev *indio_dev, struct iio_chan_spec const *channel, int *val, int *val2, long mask){
	int ret;
	mutex_lock(&indio_dev->mlock);
	struct fakeiio_state *st = iio_priv(indio_dev);
	switch (mask) {
		case IIO_CHAN_INFO_RAW:
			switch (channel->type) {
				case IIO_GENERIC_DATA:
					switch(channel->channel) {
						case 0:
							*val = st->index;
							mutex_unlock(&indio_dev->mlock);
							return IIO_VAL_INT;
							break;
						default:
							*val=0;
							mutex_unlock(&indio_dev->mlock);
							return IIO_VAL_INT;
					}
					break;
				default:
					*val=0;
					mutex_unlock(&indio_dev->mlock);
					return IIO_VAL_INT;
			}
			break;
		default:
			*val=0;
			mutex_unlock(&indio_dev->mlock);
			return IIO_VAL_INT;
	}
	
}

static int fakeiio_write_raw_iio(struct iio_dev *indio_dev, struct iio_chan_spec const *channel, int val, int val2, long mask){
	int ret;
	mutex_lock(&indio_dev->mlock);
	struct fakeiio_state *st = iio_priv(indio_dev);
	switch (mask) {
		case IIO_CHAN_INFO_RAW:
			switch (channel->type) {
				case IIO_GENERIC_DATA:
					switch(channel->channel) {
						case 0:
							st->index = val;
							mutex_unlock(&indio_dev->mlock);
							return 0;
							break;
						default:
							st->index = 0;
							mutex_unlock(&indio_dev->mlock);
							return 0;
					}
					break;
				default:
					st->index = 0;
					mutex_unlock(&indio_dev->mlock);
					return 0;
			}
			break;
		default:
			st->index = 0;
			mutex_unlock(&indio_dev->mlock);
			return 0;
	}
}

static const struct iio_info fakeiio_info = {
	.read_raw = fakeiio_read_raw_iio,
	.write_raw = fakeiio_write_raw_iio,
	.debugfs_reg_access = &fakeiio_reg_access,
	.driver_module = THIS_MODULE,
};

static const struct of_device_id fakeiio_of_match[] = {
	{ .compatible = "viettel,fakeiio", RRU_ID_FAKEIIO},
	{},
};

MODULE_DEVICE_TABLE(of, fakeiio_of_match);

static int fakeiio_probe(struct platform_device *pdev){
	int ret;
	struct of_device_id *id;
	struct iio_dev *indio_dev;
	struct fakeiio_state *st;
	struct resource res;
	struct resource *mem;

	pr_notice("Probing device %s\n", pdev->dev.of_node->name);

	id = of_match_device(fakeiio_of_match, &pdev->dev);
	if (!id)
		return -ENODEV;

	/* Allocate IIO device */
	indio_dev = devm_iio_device_alloc(&pdev->dev, sizeof(*st));
	if (!indio_dev) {
 		dev_err(&pdev->dev, "IIO allocation failed!\n");
 		return -ENOMEM;
 	}
 	st = iio_priv(indio_dev);
 	dev_set_drvdata(&pdev->dev, indio_dev);
	st->pdev = pdev;
  	
	indio_dev->name = pdev->dev.of_node->name;
	indio_dev->info = &fakeiio_info;
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->channels = fakeiio_chans;
	indio_dev->num_channels = ARRAY_SIZE(fakeiio_chans);

	pr_notice("Start ipcode FAKEIIO device registration ... ");
	ret = iio_device_register(indio_dev);
	if(ret==0){
		printk("done\n");
	}
	if(ret<0){
		printk("failed\n");
		return -ENOMEM;
	}

	return 0;
}

static int fakeiio_remove(struct platform_device *pdev){
	struct iio_dev *indio_dev = platform_get_drvdata(pdev);
	struct fakeiio_state *st = iio_priv(indio_dev);
	kfree(st);
	iio_device_unregister(indio_dev);
	return 0;
}

static struct platform_driver fakeiio_driver = {
	.driver = {
		.name = "fakeiio",
		.owner = THIS_MODULE,
		.of_match_table = fakeiio_of_match,
	},
	.probe		= fakeiio_probe,
	.remove		= fakeiio_remove,
};

module_platform_driver(fakeiio_driver);

MODULE_AUTHOR("CUONG PHAM");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("fakeiio DRIVER");


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
#include "ipcore.h"
#include "../rrudef.h"


static u32 ipcore_read(void __iomem *base_addr, u32 offset_addr){
	u32 ret;
	ret = ioread32(base_addr+offset_addr);
	return ret;
}

static int ipcore_write(void __iomem *base_addr, u32 offset_addr, u32 val){
	int ret;
	iowrite32(val, base_addr+offset_addr);
	return 0;
}

static int ipcore_reg_access(struct iio_dev *indio_dev, unsigned offset_addr, unsigned writeval, unsigned *readval){
	int ret;
	struct ipcore_state *st = iio_priv(indio_dev);
	if (readval == NULL) {
		//write value to registers
		mutex_lock(&indio_dev->mlock);
		ret = ipcore_write(st->base_addr, offset_addr,writeval);
		mutex_unlock(&indio_dev->mlock);
		return ret;
	}
	else{
		//read value from registers
		mutex_lock(&indio_dev->mlock);
		*readval = ipcore_read(st->base_addr,offset_addr);
		mutex_unlock(&indio_dev->mlock);
		if (*readval < 0) {
			pr_notice("Read registers failed\n");
			return *readval; // -ENOENT or negative value of ioread32
		}
		else{
			return 0;
		}
	}
}

static const struct iio_info ipcore_info = {
	.debugfs_reg_access = &ipcore_reg_access,
	.driver_module = THIS_MODULE,
};

static const struct of_device_id ipcore_of_match[] = {
	{ .compatible = "viettel,ipcore", RRU_ID_IPCORE},
	{},
};

MODULE_DEVICE_TABLE(of, ipcore_of_match);

static int ipcore_probe(struct platform_device *pdev){
	int ret;
	struct of_device_id *id;
	struct iio_dev *indio_dev;
	struct ipcore_state *st;
	struct resource res;

	pr_notice("Probing device %s\n", pdev->dev.of_node->name);

	id = of_match_device(ipcore_of_match, &pdev->dev);
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
  	
  	
	ret = of_address_to_resource(pdev->dev.of_node, 0, &res);
  	if (ret) {
    	pr_notice("Cannot populate resource for ipcore\n");
    	return -EAGAIN;
 	}
 	if  (!request_mem_region(res.start, resource_size(&res), pdev->dev.of_node->name)) {
    	pr_notice("Cannot request mem region\n");
    	return -EAGAIN;
  	}
  	else{
  		pr_notice("IPcore %s has start at 0x%x and end at 0x%x\n", pdev->dev.of_node->name, res.start, res.end);
  		st->base_addr = ioremap(res.start, resource_size(&res));
  	}
  	

	indio_dev->name = pdev->dev.of_node->name;
	indio_dev->info = &ipcore_info;
	indio_dev->modes = INDIO_DIRECT_MODE;

	printk("Start ipcore IIO device registration ... ");
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

static int ipcore_remove(struct platform_device *pdev){
	struct iio_dev *indio_dev = platform_get_drvdata(pdev);
	struct ipcore_state *st = iio_priv(indio_dev);
	kfree(st);
	iio_device_unregister(indio_dev);
	return 0;
}

static struct platform_driver ipcore_driver = {
	.driver = {
		.name = "ipcore",
		.owner = THIS_MODULE,
		.of_match_table = ipcore_of_match,
	},
	.probe		= ipcore_probe,
	.remove		= ipcore_remove,
};

module_platform_driver(ipcore_driver);

MODULE_AUTHOR("CUONG PHAM");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("ipcore DRIVER");


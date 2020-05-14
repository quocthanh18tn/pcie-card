#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/timer.h>
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
#include <linux/errno.h>

#include "../rrudef.h"
#include "rrugpio.h"

static const struct gpio rrugpio_pins[] = {
	{.gpio = GET_GPIO_PIN(TX12_CPL_SW),.flags  = GPIOF_OUT_INIT_LOW,},
	{.gpio = GET_GPIO_PIN(TX34_CPL_SW),.flags  = GPIOF_OUT_INIT_LOW,},  
	{.gpio = GET_GPIO_PIN(TX56_CPL_SW),.flags  = GPIOF_OUT_INIT_LOW,},  
	{.gpio = GET_GPIO_PIN(TX78_CPL_SW),.flags  = GPIOF_OUT_INIT_LOW,},  
	{.gpio = GET_GPIO_PIN(CALIB_SW_V1),.flags  = GPIOF_OUT_INIT_LOW,},
//	{.gpio = GET_GPIO_PIN(CALIB_SW_V2),.flags  = GPIOF_OUT_INIT_LOW,},
	{.gpio = GET_GPIO_PIN(LINUX_OK),.flags  = GPIOF_OUT_INIT_LOW,},
	{.gpio = GET_GPIO_PIN(WDI),.flags  = GPIOF_OUT_INIT_LOW,},  
	{.gpio = GET_GPIO_PIN(TEST_LED4),.flags  = GPIOF_OUT_INIT_LOW,},  
	{.gpio = GET_GPIO_PIN(ANTENNA_ON),.flags  = GPIOF_OUT_INIT_LOW,},  
	// {.gpio = GET_GPIO_PIN(LED1_E_STATUS_LOS),.flags  = GPIOF_OUT_INIT_LOW,},
	// {.gpio = GET_GPIO_PIN(LED2_E_STATUS_LOS),.flags  = GPIOF_OUT_INIT_LOW,},
	// {.gpio = GET_GPIO_PIN(LED3_E_STATUS_LOS),.flags  = GPIOF_OUT_INIT_LOW,},
	// {.gpio = GET_GPIO_PIN(LED4_E_STATUS_LOS),.flags  = GPIOF_OUT_INIT_LOW,},  
};

// int rru_total_pins[] = { TX12_CPL_SW, TX34_CPL_SW, TX56_CPL_SW, TX78_CPL_SW,\
//    						CALIB_SW_V1, CALIB_SW_V2,\
//    						LINUX_OK, WDI,\
//    						TEST_LED4,\
//    						ANTENNA_ON,\
//    						LED1_E_STATUS_LOS, LED2_E_STATUS_LOS, LED3_E_STATUS_LOS, LED4_E_STATUS_LOS};

int rru_total_pins[] = { TX12_CPL_SW, TX34_CPL_SW, TX56_CPL_SW, TX78_CPL_SW,\
						CALIB_SW_V1, \
						LINUX_OK, WDI,\
						TEST_LED4,\
						ANTENNA_ON};

static int rru_gpio_request(void){
	int ret = 0;
	int i=0;
	struct gpio_desc *tmp_gpio_desc;
	//ret = gpio_request_array(rrugpio_pins, ARRAY_SIZE(rrugpio_pins));
	for(i=0; i< sizeof(rru_total_pins)/sizeof(int); i++){
		ret = gpio_request(GET_GPIO_PIN(rru_total_pins[i]),"");
		if(ret<0){
			pr_notice("Request rrugpio pin %d failed with ret = %d", rru_total_pins[i], ret);
			return ret;
		}
		gpio_direction_output(GET_GPIO_PIN(rru_total_pins[i]),0);

		tmp_gpio_desc = gpio_to_desc(GET_GPIO_PIN(rru_total_pins[i]));

		if(tmp_gpio_desc){
			ret = gpiod_export(tmp_gpio_desc,false);
			if(ret<0){
				pr_notice("Export rrugpio pin %d failed with ret = %d", GET_GPIO_PIN(rru_total_pins[i]), ret);
				return ret;
			}
			else{
				pr_notice("Export rrugpio pin %d successfully\n", GET_GPIO_PIN(rru_total_pins[i]));
			}
		}
		else{
			pr_notice("tmp_gpio_desc of pin %d is NULL\n", GET_GPIO_PIN(rru_total_pins[i]));
		}
		
	}
	return ret;
}
EXPORT_SYMBOL_GPL(rru_gpio_request);

static void rru_gpio_free(void)
{
	int i=0;
    //gpio_free_array(rrugpio_pins, ARRAY_SIZE(rrugpio_pins));
    for(i=0; i< sizeof(rru_total_pins)/sizeof(int); i++){
		gpio_free(GET_GPIO_PIN(rru_total_pins[i]));
	}

}
EXPORT_SYMBOL_GPL(rru_gpio_free);

void rru_set_gpio_state(int gpio, int state){
	gpio_set_value(GET_GPIO_PIN(gpio), state);
}

EXPORT_SYMBOL_GPL(rru_set_gpio_state);

int rru_get_gpio_state(int gpio){
	int ret;
	ret = gpio_get_value(GET_GPIO_PIN(gpio));
	return ret;
}

EXPORT_SYMBOL_GPL(rru_get_gpio_state);

static int rrugpio_reg_access(struct iio_dev *indio_dev, unsigned offset_addr, unsigned writeval, unsigned *readval){
	int ret;
	int i=0;
	

	struct rrugpio_state *st = iio_priv(indio_dev);
	if (readval == NULL) {
		//write value to rrugpio
		mutex_lock(&indio_dev->mlock);
		int pin_valid = 0;
		for(i=0; i< sizeof(rru_total_pins)/sizeof(int); i++){
			if(offset_addr==rru_total_pins[i]){
				pin_valid = 1;
				rru_set_gpio_state(offset_addr,writeval);
				break;
			}
		}
		if(pin_valid)
			ret = 0;
		else
			ret = -EINVAL;

		mutex_unlock(&indio_dev->mlock);
		return ret;
	}
	else{
		//read value from rrugpio
		mutex_lock(&indio_dev->mlock);
		*readval = rru_get_gpio_state(offset_addr);
		mutex_unlock(&indio_dev->mlock);
		if (*readval < 0) {
			pr_notice("Read rrugpio failed\n");
			return *readval; // -ENOENT or negative value of ioread32
		}
		else{
			return 0;
		}
	}
}

static const struct iio_info rrugpio_info = {
	.debugfs_reg_access = &rrugpio_reg_access,
	.driver_module = THIS_MODULE,
};

static const struct of_device_id rrugpio_of_match[] = {
	{ .compatible = "viettel,rrugpio", RRU_ID_RRUGPIO},
	{},
};

MODULE_DEVICE_TABLE(of, rrugpio_of_match);

static int rrugpio_probe(struct platform_device *pdev){
	int ret;
	int i;
	struct of_device_id *id;
	struct iio_dev *indio_dev;
	struct rrugpio_state *st;
	struct resource res;

	pr_notice("Probing device %s\n", pdev->dev.of_node->name);


	id = of_match_device(rrugpio_of_match, &pdev->dev);
	if (!id)
		return -ENODEV;
		
	ret = rru_gpio_request();
	if(ret<0){
		pr_notice("rru_gpio_request with ret = %d\n", ret);
		return ret;
	}

	int pin_num = sizeof(rru_total_pins)/sizeof(int);
	
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
	indio_dev->info = &rrugpio_info;
	indio_dev->modes = INDIO_DIRECT_MODE;

	printk("Start rrugpio IIO device registration ... ");
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

static int rrugpio_remove(struct platform_device *pdev){
	struct iio_dev *indio_dev = platform_get_drvdata(pdev);
	struct rrugpio_state *st = iio_priv(indio_dev);
	rru_gpio_free();
	kfree(st);
	iio_device_unregister(indio_dev);
	return 0;
}



// static int __init rru_gpio_init(void)
// {
// 	int ret;
// 	pr_notice("rrugpio_init 11\n");
// 	rru_gpio_request();

// 	return 0;
// }


// static void __exit rru_gpio_exit(void)
// {
// 	rru_gpio_free();
// }

// fs_initcall(rru_gpio_init);
// module_exit(rru_gpio_exit);


// MODULE_AUTHOR("VIETTEL");
// MODULE_DESCRIPTION("VIETTEL RRU");
// MODULE_LICENSE("Dual BSD/GPL");


static struct platform_driver rrugpio_driver = {
	.driver = {
		.name = "rrugpio",
		.owner = THIS_MODULE,
		.of_match_table = rrugpio_of_match,
	},
	.probe		= rrugpio_probe,
	.remove		= rrugpio_remove,
};

module_platform_driver(rrugpio_driver);

MODULE_AUTHOR("CUONG PHAM");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("rrugpio DRIVER");

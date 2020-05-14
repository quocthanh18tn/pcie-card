/*
 * I2C multiplexer on CPLD
 * Based on:
 *	pca954x.c from Kumar Gala <galak@kernel.crashing.org>
 * 
 * The different between pca954x.c and cpld_i2c-mux.c is that
 * the select channel will write the value of channel directly
 * instead of bitwise conversion.
 *	
 * Also, this module does not have IRQ
 *
 * Copyright (C) 2006
 *
 */

#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/i2c-mux.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/slab.h>
#include "../rrudef.h"

#define CPLD_I2C_MUX_MAX_NCHANS 30

struct CPLDi2cMux_platform_mode {
	int		adap_id;
	unsigned int	deselect_on_exit:1;
	unsigned int	class;
};

/* Per mux/switch data, used with i2c_register_board_info */
// struct CPLDi2cMux_platform_data {
// 	struct CPLDi2cMux_platform_mode *modes;
// 	int num_modes;
// };

struct CPLDi2cMux {
	u8 nchans;
	u8 last_chan;		/* last register value */
	struct i2c_client *client;

	// struct irq_domain *irq;
	// unsigned int irq_mask;
	// raw_spinlock_t lock;
};


static const struct i2c_device_id CPLDi2cMux_id[] = {
	{ "pca_i2c-mux" },

	{ }
};
MODULE_DEVICE_TABLE(i2c, CPLDi2cMux_id);

#ifdef CONFIG_OF
static const struct of_device_id CPLDi2cMux_of_match[] = {
	{ .compatible = "viettel,pca_i2c-mux", },
	{}
};
MODULE_DEVICE_TABLE(of, CPLDi2cMux_of_match);
#endif

//  Write to mux register. Don't use i2c_transfer()/i2c_smbus_xfer()
//  for this as they will try to lock adapter a second time */
static int CPLDi2cMux_reg_write(struct i2c_adapter *adap,
			     struct i2c_client *client, u8 val)
{
	int ret = -ENODEV;

	if (adap->algo->master_xfer) {
		// pr_notice("%s: master_xfer is valid\n", __func__);
		struct i2c_msg msg;
		char buf[1];

		msg.addr = client->addr;
		msg.flags = 0;
		msg.len = 1;
		buf[0] = val;
		msg.buf = buf;
		ret = __i2c_transfer(adap, &msg, 1);

		if (ret >= 0 && ret != 1)
			ret = -EREMOTEIO;
	} else {
		union i2c_smbus_data data;
		ret = adap->algo->smbus_xfer(adap, client->addr,
					     client->flags,
					     I2C_SMBUS_WRITE,
					     val, I2C_SMBUS_BYTE, &data);
	}

	return ret;
}

static int CPLDi2cMux_select_chan(struct i2c_mux_core *muxc, u32 chan)
{
	struct CPLDi2cMux *data = i2c_mux_priv(muxc);
	struct i2c_client *client = data->client;
	//pr_notice("%s: channel %d\n", __func__, chan);

	int ret = 0;

	if (data->last_chan != chan) {
		ret = CPLDi2cMux_reg_write(muxc->parent, client, chan);
		data->last_chan = ret < 0 ? 0 : chan;
	}

	return ret;
}

static int CPLDi2cMux_deselect_mux(struct i2c_mux_core *muxc, u32 chan)
{
	//pr_notice("%s: channel %d\n", __func__, chan);
	struct CPLDi2cMux *data = i2c_mux_priv(muxc);
	struct i2c_client *client = data->client;


	data->last_chan = 0;
	return CPLDi2cMux_reg_write(muxc->parent, client, data->last_chan);
}

/*
 * I2C init/probing/exit functions
 */
static int CPLDi2cMux_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	pr_notice("%s: %s starts\n", __func__, client->name);

	struct i2c_adapter *adap = to_i2c_adapter(client->dev.parent);
	// struct CPLDi2cMux_platform_data *pdata = dev_get_platdata(&client->dev);
	struct device_node *of_node = client->dev.of_node;
	int num, force, class;
	struct i2c_mux_core *muxc;
	struct CPLDi2cMux *data;
	const struct of_device_id *match;
	int ret;

	ret = i2c_check_functionality(adap, I2C_FUNC_SMBUS_BYTE);
	if (!ret){
		pr_err("%s: Failed to check functionality (%d)\n", __func__, ret);
		return -ENODEV;
	}

	muxc = i2c_mux_alloc(adap, &client->dev,
			     CPLD_I2C_MUX_MAX_NCHANS, sizeof(*data), 0,
			     CPLDi2cMux_select_chan, CPLDi2cMux_deselect_mux);
	if (!muxc){
		pr_err("%s: Failed to allocate i2c_mux (%d)\n", __func__, muxc);
		return -ENOMEM;
	}
	data = i2c_mux_priv(muxc);

	i2c_set_clientdata(client, muxc);
	data->client = client;
	data->nchans = CPLD_I2C_MUX_MAX_NCHANS;

	/* Write the mux register at addr to verify
	 * that the mux is in fact present. This also
	 * initializes the mux to disconnected state.
	 */
	unsigned char temp = 0x00;
	ret = i2c_master_send(client, &temp, 1);
	if ( ret < 0) {
		dev_err(&client->dev, "probe failed (%d)\n", ret);
		return -ENODEV;
	}

	data->last_chan = 0;		   /* force the first selection */
	force = 0;			  /* dynamic adap number */
	class = 0;			  /* no class by default */
	/* Now create an adapter for each channel */
	for (num = 0; num < data->nchans; num++) {
		ret = i2c_mux_add_adapter(muxc, force, num, class);
		if (ret)
			goto fail_del_adapters;
	}

	dev_info(&client->dev,
		 "registered %d multiplexed busses for I2C %s %s\n",
		 num, "mux", client->name);

	return 0;

fail_del_adapters:
	i2c_mux_del_adapters(muxc);
	return ret;
}

static int CPLDi2cMux_remove(struct i2c_client *client)
{
	struct i2c_mux_core *muxc = i2c_get_clientdata(client);
	i2c_mux_del_adapters(muxc);
	return 0;
}

static struct i2c_driver CPLDi2cMux_driver = {
	.driver		= {
		.name	= "cpld_i2c-mux",
		#ifdef CONFIG_OF
		.of_match_table = of_match_ptr(CPLDi2cMux_of_match),
		#endif
	},
	.probe		= CPLDi2cMux_probe,
	.remove		= CPLDi2cMux_remove,
	.id_table	= CPLDi2cMux_id,
};

module_i2c_driver(CPLDi2cMux_driver);

MODULE_AUTHOR("Pham Xuan Tra <trapx@viettel.com.vn>");
MODULE_DESCRIPTION("CPLD I2C mux/switch driver");
MODULE_LICENSE("GPL v2");
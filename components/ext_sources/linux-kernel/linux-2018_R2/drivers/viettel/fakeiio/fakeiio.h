#ifndef __FAKEIIO_H__
#define __FAKEIIO_H__

#define TRUE 1
#define FALSE 0


struct fakeiio_state {
	struct platform_device *pdev;
	u32 fake_regs[100];
	u32 index;
};


/************************************************************************************************************** 
//Function: fakeiio_reg_access
//purpose: call back for iio debug fs
//parameter:
//  indio_dev: iio_dev structure
//  offset_addr: expected offset address that need to be interact
//  writeval: value to be written to addr
//  readval: a pointer to contain read value
//
//return: 0 if success, negative number if failed
***************************************************************************************************************/
static int fakeiio_reg_access(struct iio_dev *indio_dev, unsigned offset_addr, unsigned writeval, unsigned *readval);


static int fakeiio_read_raw_iio(struct iio_dev *indio_dev, struct iio_chan_spec const *channel, int *val, int *val2, long mask);
static int fakeiio_write_raw_iio(struct iio_dev *indio_dev, struct iio_chan_spec const *channel, int val, int val2, long mask);

static const struct iio_chan_spec fakeiio_chans[] = {
	{
		.type = IIO_GENERIC_DATA,
		.indexed = 1,
		.channel = 0,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),		
	},
};
#endif

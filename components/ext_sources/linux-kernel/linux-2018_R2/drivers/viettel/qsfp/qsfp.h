#ifndef __QSFP_H__
#define __QSFP_H__

#define SFP_VENDOR_INFO 0x50
#define SFP_DIAGNOSTICS 0x51

typedef enum{
	TYPE_VENDOR = 1,
	TYPE_DIAGNOSTIC = 2,
} sfp_type_t;

struct qsfp_state {
    struct i2c_client *client;
    sfp_type_t type;
};

/************************************************************************************************************** 
//Function: qsfp_read
//purpose: read value from expected address
//parameter:
//  client: i2c_client structure
//  addr: expected address that need to be read value
//
//return: 0 if success, negative number if failed
***************************************************************************************************************/
static int qsfp_read(struct i2c_client *client, unsigned addr);

/************************************************************************************************************** 
//Function: qsfp_write
//purpose: write value to expected address
//parameter:
//  client: i2c_client structure
//  addr: expected address that need to be write value
//  value: value to be written to addr
//
//return: 0 if success, negative number if failed
***************************************************************************************************************/
static int qsfp_write(struct i2c_client *client, unsigned addr, unsigned value);

/************************************************************************************************************** 
//Function: qsfp_reg_access
//purpose: call back for iio debug fs
//parameter:
//  indio_dev: iio_dev structure
//  reg: expected reg that need to be interact
//  writeval: value to be written to addr
//  readval: a pointer to contain read value
//
//return: 0 if success, negative number if failed
***************************************************************************************************************/
static int qsfp_reg_access(struct iio_dev *indio_dev, unsigned reg, unsigned writeval, unsigned *readval);

#endif

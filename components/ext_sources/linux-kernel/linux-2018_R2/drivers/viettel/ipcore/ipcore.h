#ifndef __IPCORE_H__
#define __IPCORE_H__

struct ipcore_state {
	struct platform_device *pdev;
	void __iomem *base_addr;
};

/************************************************************************************************************** 
//Function: ipcore_oai_read
//purpose: read value from expected offset address
//parameter:
//  base_addr: base address of memory area
//  offset_addr: expected offset address that need to be read value
//
//return: 0 if success, negative number if failed
***************************************************************************************************************/
static u32 ipcore_read(void __iomem *base_addr, u32 offset_addr);

/************************************************************************************************************** 
//Function: ipcore_write
//purpose: read value from expected offset address
//parameter:
//  base_addr: base address of memory area
//  offset_addr: expected offset address that need to be write value
//	value: value to be written to offset_addr
//
//return: 0 if success, negative number if failed
***************************************************************************************************************/
static int ipcore_write(void __iomem *base_addr, u32 offset_addr, u32 val);

/************************************************************************************************************** 
//Function: ipcore_reg_access
//purpose: call back for iio debug fs
//parameter:
//  indio_dev: iio_dev structure
//  offset_addr: expected offset address that need to be interact
//  writeval: value to be written to addr
//  readval: a pointer to contain read value
//
//return: 0 if success, negative number if failed
***************************************************************************************************************/
static int ipcore_reg_access(struct iio_dev *indio_dev, unsigned offset_addr, unsigned writeval, unsigned *readval);

#endif

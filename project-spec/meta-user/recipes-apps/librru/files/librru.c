#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/socket.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <iio.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>
#include "librru.h"
#include "rrudef.h"
#include "rrulocal.h"


static int hw_version = 1;
static struct iio_context *ctx;

int iio_open_device(){
    ctx = iio_create_local_context();
    if (!ctx) {
        LOG_DEBUG("Unable to create context");
        return -EXIT_FAILURE;
    }
    return 0;
}

int iio_close_device(){
    if (!ctx) {
        LOG_DEBUG("Unable to create context");
        return -EXIT_FAILURE;
    }
    iio_context_destroy(ctx);
    return 0;
}

int rru_open_device(){
    return iio_open_device();  
}

int rru_close_device(){
    return iio_close_device();
}

struct iio_context * get_rru_iio_ctx(void){
    return ctx;
}

static int iio_write_reg(const char *name, unsigned long addr, unsigned long val)
{
    struct iio_device *dev;
    int ret;

    if (!ctx) {
        LOG_DEBUG("IIO context for %s not yet open!");
        return -EXIT_FAILURE;
    }
 
    dev = iio_context_find_device(ctx, name);
    if (!dev) {
        LOG_DEBUG("Unable to find device %s\n", name);
        return -1;
    }

    ret = iio_device_reg_write(dev, addr, val);
    if (ret < 0) {
        LOG_DEBUG("Unable to write register");
    }
    return ret;
}

static int iio_read_reg(const char *name, unsigned long addr)
{
    struct iio_device *dev;
    uint32_t val;
    int ret;

    if (!ctx) {
        LOG_DEBUG("IIO context for %s not yet open!");
        return -EXIT_FAILURE;
    }

    dev = iio_context_find_device(ctx, name);
    if (!dev) {
        LOG_DEBUG("Unable to find device %s", name);
        return -1;
    }
 
    ret = iio_device_reg_read(dev, addr, &val);
    if (ret < 0) {
        LOG_DEBUG("Unable to read register %s[%d]", name, addr);
        return -1;
    }

    return val;
}

static int iio_write_attr(char *devname, char *attr, char *val)
{
    struct iio_device *dev;
    struct iio_channel *ch = NULL;
    int ret;
    int i; 
    int channel_exist;

    if (!ctx) {
        LOG_DEBUG("IIO context for %s not yet open!");
        printf("IIO context not yet open\n");
        return -EXIT_FAILURE;
    }
 
    dev = iio_context_find_device(ctx, devname);
    if (!dev) {
        LOG_DEBUG("Unable to find device %s\n", devname);
        printf("Unable to find device\n");
        return -1;
    }

    ret = iio_device_attr_write(dev, attr, val);
    if (ret < 0) {
        LOG_DEBUG("Unable to write channel");
        printf("Unable to write channel\n");
    }
    return ret;
}

static unsigned iio_read_attr(char *devname, char *attr, char *val)
{
    struct iio_device *dev;
    struct iio_channel *ch = NULL;
    int ret;
    int i; 
    int channel_exist;
    char buf[1024];

    if (!ctx) {
        LOG_DEBUG("IIO context for %s not yet open!");
        printf("IIO context not yet open\n");
        return -EXIT_FAILURE;
    }
 
    dev = iio_context_find_device(ctx, devname);
    if (!dev) {
        LOG_DEBUG("Unable to find device %s\n", devname);
        printf("Unable to write channel\n");
        return -1;
    }

    ret = iio_device_attr_read(dev, attr, buf, sizeof(buf));
    if (ret < 0){
        LOG_DEBUG("Unable to read channel");
        printf("Unable to read channel\n");
        return ret;
    }

    strcpy(val,buf);
    return ret;

}

int get_rf_device_name(int id, char* name){
    switch(id){
        case RRU_ID_FAKEIIO :
            strcpy(name, "fakeiio");
            break;
        case RRU_ID_LMK05028 :
            strcpy(name, "lmk05028");
            break;
        case RRU_ID_HMC7044 :
            strcpy(name, "hmc7044");
            break;
        case RRU_ID_HMC7043 :
            strcpy(name, "hmc7043");
            break;
        case RRU_ID_SFP_1_VENDOR :
            strcpy(name, "sfp_1_vendor");
            break;
        case RRU_ID_SFP_1_DIAGNOSTIC :
            strcpy(name, "sfp_1_diagnostic");
            break;
        case RRU_ID_SFP_2_VENDOR :
            strcpy(name, "sfp_2_vendor");
            break;
        case RRU_ID_SFP_2_DIAGNOSTIC :
            strcpy(name, "sfp_2_diagnostic");
            break;
        case RRU_ID_SFP_3_VENDOR :
            strcpy(name, "sfp_3_vendor");
            break;
        case RRU_ID_SFP_3_DIAGNOSTIC :
            strcpy(name, "sfp_3_diagnostic");
            break;
        case RRU_ID_SFP_4_VENDOR :
            strcpy(name, "sfp_4_vendor");
            break;
        case RRU_ID_SFP_4_DIAGNOSTIC :
            strcpy(name, "sfp_4_diagnostic");
            break;
        case RRU_ID_RRUGPIO :
            strcpy(name, "rrugpio");
            break;
        case RRU_ID_IPCORE :
            strcpy(name, "ipcore");
            break;
        case RRU_ID_ADRV9009_TRX12 :
            strcpy(name, "adrv9009_phy_trx12");
            break;
        case RRU_ID_ADRV9009_TRX34 :
            strcpy(name, "adrv9009_phy_trx34");
            break;
        case RRU_ID_ADRV9009_TRX56 :
            strcpy(name, "adrv9009_phy_trx56");
            break;
        case RRU_ID_ADRV9009_TRX78 :
            strcpy(name, "adrv9009_phy_trx78");
            break;
        case RRU_ID_ADRV9009_CALIB :
            strcpy(name, "adrv9009_phy_calib");
            break;
        default :
            return -1;
            break;
    }
    //printf ("IIO device: %d %s",id,name);   
    return 0;
}

/* This function is for IIC devices that do not use IIO framwork */
int get_i2c_device_file(int id, char* iic_devfile, int *dev_address){
    switch(id){
        case RRU_ID_ADT7420 :
            strcpy(iic_devfile, "/dev/i2c-10");
            *dev_address = 0x48;
            break;
        case RRU_ID_INA_12V :
            strcpy(iic_devfile, "/dev/i2c-11");
            *dev_address = 0x40;
            break;
        case RRU_ID_INA_28V :
            strcpy(iic_devfile, "/dev/i2c-12");
            *dev_address = 0x41;
            break;
        case RRU_ID_INA_3V3_VCCO :
            strcpy(iic_devfile, "/dev/i2c-13");
            *dev_address = 0x40;
            break;
        case RRU_ID_INA_1V8_VCCAUX :
            strcpy(iic_devfile, "/dev/i2c-14");
            *dev_address = 0x40;
            break;
        case RRU_ID_INA_1V8_VCCO :
            strcpy(iic_devfile, "/dev/i2c-15");
            *dev_address = 0x40;
            break;
        case RRU_ID_INA_1V2_VMGTAVTT :
            strcpy(iic_devfile, "/dev/i2c-16");
            *dev_address = 0x40;
            break;
        case RRU_ID_INA_1V2_VCCO :
            strcpy(iic_devfile, "/dev/i2c-17");
            *dev_address = 0x40;
            break;
        case RRU_ID_INA_0V9_VMGTAVCC :
            strcpy(iic_devfile, "/dev/i2c-18");
            *dev_address = 0x40;
            break;
        case RRU_ID_INA_0V85_VCCPSINTLP :
            strcpy(iic_devfile, "/dev/i2c-19");
            *dev_address = 0x40;
            break;
        case RRU_ID_INA_0V85_VCCINT :
            strcpy(iic_devfile, "/dev/i2c-20");
            *dev_address = 0x40;
            break;
        case RRU_ID_INA_0V85_VCCPSINTFP :
            strcpy(iic_devfile, "/dev/i2c-21");
            *dev_address = 0x40;
            break;
        case RRU_ID_INA_28V_PA :
            strcpy(iic_devfile, "/dev/i2c-22");
            *dev_address = 0x40;
            break;
        case RRU_ID_LM73 :
            strcpy(iic_devfile, "/dev/i2c-12");
            *dev_address = 0x4A;
            break;
        case RRU_ID_PCA9551 :
            strcpy(iic_devfile, "/dev/i2c-12");
            *dev_address = 0x60;
            break;
        case RRU_ID_PA :
            strcpy(iic_devfile, "/dev/i2c-8");
            *dev_address = 0x18;
            break;
        default :
            return -1;
            break;
    }
    //printf ("IIO device: %d %s",id,iic_devfile);   
    return 0;
}

int rru_write_reg(int id, unsigned reg, unsigned value){
    char device_name[20];
    char iic_devfile_name[20];
    int dev_address;
    unsigned char write_buf[4];
    int write_len;
    int fd;
    int ret;

    if((id != RRU_ID_INA_12V) && \
    (id != RRU_ID_INA_28V) && \ 
    (id != RRU_ID_INA_3V3_VCCO) && \
    (id != RRU_ID_INA_1V8_VCCAUX) && \
    (id != RRU_ID_INA_1V8_VCCO) && \
    (id != RRU_ID_INA_1V2_VMGTAVTT) && \
    (id != RRU_ID_INA_1V2_VCCO) && \
    (id != RRU_ID_INA_0V9_VMGTAVCC) && \
    (id != RRU_ID_INA_0V85_VCCPSINTLP) && \
    (id != RRU_ID_INA_0V85_VCCINT) && \
    (id != RRU_ID_INA_0V85_VCCPSINTFP) && \
    (id != RRU_ID_INA_28V_PA) && \
    (id != RRU_ID_ADT7420) && \
    (id != RRU_ID_LM73) && \
    (id != RRU_ID_PA) && \
    (id != RRU_ID_PCA9551)){
        ret = get_rf_device_name(id, device_name);
        if(!ret){
            return iio_write_reg(device_name, reg, value);
        }
        printf("Unable to find device id = %d\n", id);
        return ret;
    }
    else{
        ret = get_i2c_device_file(id, iic_devfile_name, &dev_address);
        if(!ret){
            fd = open(iic_devfile_name, O_RDWR);
            if(fd < 0){
                printf("Unable to open I2C device file %s with errno = %d\n", iic_devfile_name, errno);
                ret = -1;
                return ret;
            }
            ret = ioctl(fd, I2C_SLAVE_FORCE, dev_address);
            if (ret < 0) {
                printf("Failed to acquire bus access and/or talk to slave address %d with errno = %d\n", dev_address, errno);
                ret = -1;
                return ret;
            }
            
            if(id == RRU_ID_LM73){
                if((reg == 0x01) || (reg == 0x03)){
                    write_buf[0] = reg & 0xFF;
                    write_buf[1] = value & 0xFF;
                    write_len = 2;
                }
                else{
                    write_buf[0] = reg & 0xFF;
                    write_buf[1] = (value >> 8) & 0xFF;
                    write_buf[2] = value & 0xFF;
                    write_len = 3;
                }
            }
            else if(id == RRU_ID_ADT7420){
                write_buf[0] = reg & 0xFF;
                write_buf[1] = value & 0xFF;
                write_len = 2;
            }
            else if(id == RRU_ID_PA){
                printf("Write to PA is not supported\n");
                ret = 0;
                return ret;
            }
            else if(id == RRU_ID_PCA9551){
                write_buf[0] = reg & 0xFF;
                write_buf[1] = value & 0xFF;
                write_len = 2; 
            }
            else{
                // INA226
                write_buf[0] = reg & 0xFF;
                write_buf[1] = (value >> 8) & 0xFF;
                write_buf[2] = value & 0xFF;
                write_len = 3;
            }

            struct i2c_rdwr_ioctl_data i2c_data;
            struct i2c_msg msgs;

            msgs.addr = dev_address;
            msgs.flags = 0;
            msgs.buf = write_buf;
            msgs.len = write_len;
            i2c_data.msgs = &msgs;
            i2c_data.nmsgs = 1;

            ret = ioctl(fd, I2C_RDWR, &i2c_data);

            if(ret < 0){
                printf("Unable to write to slave address %d on bus %s with errno = %d\n", dev_address, iic_devfile_name, errno);
                ret = -1;
                return ret;
            }
            else
                return 0;
        }
        printf("Unable to find device id = %d\n", id);
        return ret;
        
    }
    
}

int rru_read_reg(int id, unsigned reg){
    char device_name[20];
    char iic_devfile_name[20];
    int dev_address;
    unsigned char write_buf[4];
    unsigned char read_buf[32];
    int write_len = 1;
    int read_len;
    int fd;
    int ret;

    if((id != RRU_ID_INA_12V) && \
    (id != RRU_ID_INA_28V) && \ 
    (id != RRU_ID_INA_3V3_VCCO) && \
    (id != RRU_ID_INA_1V8_VCCAUX) && \
    (id != RRU_ID_INA_1V8_VCCO) && \
    (id != RRU_ID_INA_1V2_VMGTAVTT) && \
    (id != RRU_ID_INA_1V2_VCCO) && \
    (id != RRU_ID_INA_0V9_VMGTAVCC) && \
    (id != RRU_ID_INA_0V85_VCCPSINTLP) && \
    (id != RRU_ID_INA_0V85_VCCINT) && \
    (id != RRU_ID_INA_0V85_VCCPSINTFP) && \
    (id != RRU_ID_INA_28V_PA) && \
    (id != RRU_ID_ADT7420) && \
    (id != RRU_ID_LM73) && \
    (id != RRU_ID_PA) && \
    (id != RRU_ID_PCA9551)){
        ret = get_rf_device_name(id, device_name);
        if(!ret){
            return iio_read_reg(device_name, reg);
        }
        printf("Unable to find device id = %d\n", id);
        return ret;
    }
    else{
        ret = get_i2c_device_file(id, iic_devfile_name, &dev_address);
        if(!ret){
            fd = open(iic_devfile_name, O_RDWR);
            if(fd < 0){
                printf("Unable to open I2C device file %s with errno = %d\n", iic_devfile_name, errno);
                ret = -1;
                return ret;
            }
            ret = ioctl(fd, I2C_SLAVE_FORCE, dev_address);
            if (ret < 0) {
                printf("Failed to acquire bus access and/or talk to slave address %d with errno = %d\n", dev_address, errno);
                ret = -1;
                return ret;
            }

            write_buf[0] = reg & 0xFF;
            // ret = write(fd, write_buf, write_len);
            // if(ret != write_len){
            //     printf("Unable to write to slave address %d on bus %s with errno = %d\n", dev_address, iic_devfile_name, errno);
            //     ret = -1;
            //     return ret;
            // }
            struct i2c_rdwr_ioctl_data i2c_data;
            struct i2c_msg msgs[2];
            i2c_data.msgs = msgs;
            i2c_data.nmsgs = 2;

            msgs[0].addr = dev_address;
            msgs[0].len = 1;
            msgs[0].buf = write_buf;
            msgs[0].flags = 0;

            if(id == RRU_ID_LM73){
                if((reg == 0x01) || (reg == 0x03)){
                    read_len = 1;
                }
                else{
                    read_len = 2;
                }
            }
            else if(id == RRU_ID_ADT7420){
                read_len = 1;
            }
            else if(id == RRU_ID_PA){
                if(reg == 0x0A){
                    read_len = 9;
                }
                else if(reg == 0x0B){
                    read_len = 6;
                }
                else if((reg == 0x0C) || (reg == 0x0E) || (reg == 0x0F)){
                    read_len = 8;
                }
                else { // if((reg == 0x0D) || (reg == 0x10) || (reg == 0x15))
                    read_len = 1;
                }
            }
            else if(id == RRU_ID_PCA9551){
                read_len = 1; 
            }
            else{
                // INA226
                read_len = 2;
            }

            // ret = read(fd, read_buf, read_len);
            msgs[1].addr = dev_address;
            msgs[1].len = read_len;
            msgs[1].buf = read_buf;
            msgs[1].flags = I2C_M_RD;
            ret = ioctl(fd, I2C_RDWR, &i2c_data);

            if(ret < 0){
                printf("Unable to write to slave address %d on bus %s with errno = %d-%d\n", dev_address, iic_devfile_name, ret, errno);
                ret = -1;
            }else{
                if(id != RRU_ID_PA){
                    if(read_len == 1)
                        return read_buf[0];
                    if(read_len == 2)
                        return read_buf[0]*256+read_buf[1];
                }
                else{
                    if(read_len == 1){
                        printf("Read from PA: 0x%X\n", read_buf[0]);
                    }
                    if(read_len == 6){
                        printf("Read from PA: 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n", read_buf[0], read_buf[1], read_buf[2], read_buf[3], read_buf[4], read_buf[5]);
                    }
                    if(read_len == 8){
                        printf("Read from PA: 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n", read_buf[0], read_buf[1], read_buf[2], read_buf[3], read_buf[4], read_buf[5], read_buf[6], read_buf[7]);
                    }
                    if(read_len == 9){
                        printf("Read from PA: 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n", read_buf[0], read_buf[1], read_buf[2], read_buf[3], read_buf[4], read_buf[5], read_buf[6], read_buf[7], read_buf[8]);
                    }
                }
                return ret;
            }
        }
        printf("Unable to find device id = %d\n", id);
        return ret;
        
    }
}

int rru_write_attr(int id, char *attr, char *value){
    char device_name[20];
    int ret = get_rf_device_name(id, device_name);
    if(!ret){
        return iio_write_attr(device_name, attr, value);
    }
    printf("Unable to find device id = %d\n", id);
    return ret;
}

int rru_read_attr(int id, char *attr, char *value){
    char device_name[20];
    int ret = get_rf_device_name(id, device_name);
    if(!ret){
        return iio_read_attr(device_name, attr, value);
    }
    printf("Unable to find device id = %d\n", id);
    return ret;
}

int get_hardware_version(void){
    return hw_version;
}

void rru_safe_reboot(void){
    system("killall -KILL dhcpcd");
    system("killall inetd");
    system("killall httpd");
    system("killall ash");
    system("killall rru_manager.sh");
    system("killall orru_mon");
    system("killall klogd");
    system("killall syslogd");
    system("killall perl");
    system("killall dropbear");
    system("killall rrumon ; killall orru; reboot");
}

void checklibrruversion() {
    // Will update LOCAL_APPDATE, SVN_APPVER, SVN_APPDATE like 4G source
    printf("Build date: %s\t%s\n", __DATE__, __TIME__);
}
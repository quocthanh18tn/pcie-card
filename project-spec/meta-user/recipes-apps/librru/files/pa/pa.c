#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdint.h>
#include "pa.h"

int open_padev(pa_device *pa_dev){
    pa_dev->file = open("/dev/i2c-8", O_RDWR);
    if (pa_dev->file < 0){
        printf("open_padev: failed to open the bus: %d\n", errno);
        if (errno == EACCES)
            printf("EACCES\n");
        return errno;
    }

    if (ioctl(pa_dev->file, I2C_SLAVE, pa_dev->addr) < 0){
        printf("open_padev: failed to acquire bus access talk to slave: %d\n", errno);
        return errno;
    }

    if (ioctl(pa_dev->file, I2C_TIMEOUT, 20) < 0){
        printf("open_padev: %d\n", errno);
        return errno;
    }
    return 0;
}

int close_padev(pa_device *pa_dev){
    int ret;
    ret = close(pa_dev->file);
    if (ret){
        printf("close_padev: failed to close the bus: %d\n", errno);
        return errno;
    }
    return 0;
}

int start_padev(pa_device *pa_dev){
    int ret;
    uint8_t command;
    struct i2c_msg msgs;
    struct i2c_rdwr_ioctl_data rdwr;

    command = 0x15;

    msgs.buf = NULL;
    memset(&msgs, 0, sizeof(struct i2c_msg));
    memset(&rdwr, 0, sizeof(struct i2c_rdwr_ioctl_data));
    /* first message */
    msgs.addr = pa_dev->addr;
    msgs.flags = 0;
    msgs.len = 1;
    msgs.buf = &command;
    /* package i2c message */
    rdwr.msgs = &msgs;
    rdwr.nmsgs = 1;

    if (ret = ioctl(pa_dev->file, I2C_RDWR, (unsigned long)&rdwr) < 1){
        printf("start_padev: failed to get start data: %d\n%s\t%d\n", ret, strerror(errno), errno);
        return errno;
    }
    return 0;
}

int get_bias_status_padev(pa_device *pa_dev, uint8_t *buf){
    int ret;
    uint8_t command;
    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data rdwr;

    command = 0x10;

    msgs[0].buf = msgs[1].buf = NULL;
    memset(&msgs[0], 0, sizeof(struct i2c_msg));
    memset(&msgs[1], 0, sizeof(struct i2c_msg));
    memset(&rdwr, 0, sizeof(struct i2c_rdwr_ioctl_data));
    /* fist message */
    msgs[0].addr= pa_dev->addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &command;
    /* second message */
    msgs[1].addr = pa_dev->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 1;
    msgs[1].buf = buf;
    /* package i2c message */
    rdwr.msgs = msgs;
    rdwr.nmsgs = 2;

    if (ret = ioctl(pa_dev->file, I2C_RDWR, (unsigned long)&rdwr) < 2){
        printf("get_bias_status_padev: failed to get bias status: %d\n%s\n", ret, strerror(errno));
        return errno;
    }
    return 0;
}

int get_temp_padev(pa_device *pa_dev, float *buf){
    int i, ret;
    uint8_t command, raw_data[8];
    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data rdwr;

    command = 0x0c;

    msgs[0].buf = msgs[1].buf = NULL;
    memset(&msgs[0], 0, sizeof(struct i2c_msg));
    memset(&msgs[1], 0, sizeof(struct i2c_msg));
    memset(&rdwr, 0, sizeof(struct i2c_rdwr_ioctl_data));
    /* first message */
    msgs[0].addr = pa_dev->addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &command;
    /* second message */
    msgs[1].addr = pa_dev->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 8;
    msgs[1].buf = raw_data;
    /* package i2c message */
    rdwr.msgs = msgs;
    rdwr.nmsgs = 2;

    if (ret = ioctl(pa_dev->file, I2C_RDWR, (unsigned long)&rdwr) < 2){
        printf("get_temp_padev: failed to get temp: %d\n%s\n", ret, strerror(errno));
        return errno;
    }

    for (i = 0; i < 4; i++)
        *(buf + i) = ((float)(((raw_data[2*i] & 0x0f) << 8) + raw_data[2*i+1] - 500)) * 0.1;

    return 0;
}

int get_volt_padev(pa_device *pa_dev, float *buf){
    int ret;
    uint8_t command, raw_data[2];
    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data rdwr;

    command = 0x0d;

    msgs[0].buf = msgs[1].buf = NULL;
    memset(&msgs[0], 0, sizeof(struct i2c_msg));
    memset(&msgs[1], 0, sizeof(struct i2c_msg));
    memset(&rdwr, 0, sizeof(struct i2c_rdwr_ioctl_data));
    /* fist message */
    msgs[0].addr = pa_dev->addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &command;
    /* second message */
    msgs[1].addr = pa_dev->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 2;
    msgs[1].buf = raw_data;
    /* package i2c message */
    rdwr.msgs = msgs;
    rdwr.nmsgs = 2;

    if (ret = ioctl(pa_dev->file, I2C_RDWR, (unsigned long)&rdwr) < 2){
        printf("get_volt_padev: failed to get volt: %d\n%s\n", ret, strerror(errno));
        return errno;
    }

    *buf = ((float)(((raw_data[0] & 0x0f) << 8) + raw_data[1])) * 0.022;
    return 0;
}

int get_curr_padev(pa_device *pa_dev, float *buf){
    int i, ret;
    uint8_t command1, command2, raw_data1[8], raw_data2[8];
    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data rdwr;

    command1 = 0x0e;
    command2 = 0x0f;

    msgs[0].buf = msgs[1].buf = NULL;
    memset(&msgs[0], 0, sizeof(struct i2c_msg));
    memset(&msgs[1], 0, sizeof(struct i2c_msg));
    memset(&rdwr, 0, sizeof(struct i2c_rdwr_ioctl_data));
    /* fist message */
    msgs[0].addr = pa_dev->addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &command1;
    /* second message */
    msgs[1].addr = pa_dev->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 8;
    msgs[1].buf = raw_data1;
    /* package i2c message */
    rdwr.msgs = msgs;
    rdwr.nmsgs = 2;

    if (ret = ioctl(pa_dev->file, I2C_RDWR, (unsigned long)&rdwr) < 2){
        printf("get_curr_padev: failed to get curr1: %d\n%s\n", ret, strerror(errno));
        printf("%s\n", strerror(ret));
        return errno;
    }

    msgs[0].buf = msgs[1].buf = NULL;
    memset(&msgs[0], 0, sizeof(struct i2c_msg));
    memset(&msgs[1], 0, sizeof(struct i2c_msg));
    memset(&rdwr, 0, sizeof(struct i2c_rdwr_ioctl_data));
    /* fist message */
    msgs[0].addr = pa_dev->addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &command2;
    /* second message */
    msgs[1].addr = pa_dev->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 8;
    msgs[1].buf = raw_data2;
    /* package i2c message */
    rdwr.msgs = msgs;
    rdwr.nmsgs = 2;

    if (ret = ioctl(pa_dev->file, I2C_RDWR, (unsigned long)&rdwr) < 2){
        printf("get_curr_padev: failed to get curr2: %d\n%s\n", ret, strerror(errno));
        return errno;
    }

    for (i = 0; i < 4; i++){
        *(buf + i) = ((float)(((raw_data1[2*i] & 0x0f) << 8) + raw_data1[2*i+1])) / 1.5;
        *(buf + 4 + i) == ((float)(((raw_data2[2*i] & 0x0f) << 8) + raw_data2[2*i+1])) / 1.5;
    }

    return 0;
}
int get_vendor_padev(pa_device *pa_dev, uint8_t *buf){
    int ret;
    uint8_t command;
    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data rdwr;

    command = 0x0a;

    msgs[0].buf = msgs[1].buf = NULL;
    memset(&msgs[0], 0, sizeof(struct i2c_msg));
    memset(&msgs[1], 0, sizeof(struct i2c_msg));
    memset(&rdwr, 0, sizeof(struct i2c_rdwr_ioctl_data));
    /* first message */
    msgs[0].addr = pa_dev->addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &command;
    /* second message */
    msgs[1].addr = pa_dev->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 9;
    msgs[1].buf = buf;
    /* package i2c msg */
    rdwr.msgs = msgs;
    rdwr.nmsgs = 2;

    if (ret = ioctl(pa_dev->file, I2C_RDWR, (unsigned long)&rdwr) < 2){
        printf("get_vendor_padev: failed to read vendor: %d\n%s\n", ret, strerror(errno));
        return errno;
    }
    return 0;
}

int get_date_padev(pa_device *pa_dev, uint8_t *buf){
    int ret;
    uint8_t command;
    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data rdwr;

    command = 0x0b;

    msgs[0].buf = msgs[1].buf = NULL;
    memset(&msgs[0], 0, sizeof(struct i2c_msg));
    memset(&msgs[1], 0, sizeof(struct i2c_msg));
    memset(&rdwr, 0, sizeof(struct i2c_rdwr_ioctl_data));
    /* first message */
    msgs[0].addr = pa_dev->addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &command;
    /* second message */
    msgs[1].addr = pa_dev->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 6;
    msgs[1].buf = buf;
    /* package i2c msg */
    rdwr.msgs = msgs;
    rdwr.nmsgs = 2;

    if (ret = ioctl(pa_dev->file, I2C_RDWR, (unsigned long)&rdwr) < 2){
        printf("get_date_padev: failed to read date: %d\n%s\n", ret, strerror(errno));
        return errno;
    }
    return 0;
}

int get_pa_info(pa_info *info){
    int ret;
    pa_device pa = {
        .addr = 0x18,
    };
    pa_device *pa_dev = &pa;
    pa_info t_info;
    memset(&t_info, 0, sizeof(pa_info));

    ret = open_padev(pa_dev);
    if (ret)
        printf("%s\n", strerror(errno));
    ret = get_vendor_padev(pa_dev, t_info.vendor);
    if (ret)
        printf("%s\n", strerror(errno));
    ret = get_date_padev(pa_dev, t_info.date);
    if (ret)
        printf("%s\n", strerror(errno));
    ret = close_padev(pa_dev);
    if (ret)
        printf("%s\n", strerror(errno));
    *info = t_info;
    return 0;
}

int get_pa_status(pa_status *status){
    int ret;
    pa_device pa_dev = {
        .addr = 0x18,
    };
    pa_status t_status;
    memset(&t_status, 0, sizeof(pa_status));

    ret = open_padev(&pa_dev);
    if (ret)
        return errno;
    ret = start_padev(&pa_dev);
    if (ret)
        return errno;
    ret = get_bias_status_padev(&pa_dev, &t_status.bias_status);
    if (ret)
        return errno;
    ret = get_temp_padev(&pa_dev, t_status.temperature);
    if (ret)
        return errno;
    ret = get_volt_padev(&pa_dev, &t_status.voltage);
    if (ret)
        return errno;
    ret = get_curr_padev(&pa_dev, t_status.current);
    if (ret)
        return errno;
    ret = close_padev(&pa_dev);
    if (ret)
        return errno;
    *status = t_status;
    return 0;
}

int pa_selftest(char *vendor){
    int ret;
    uint8_t r_vendor[10];
    pa_device pa_dev = {
        .addr = 0x18,
    };
    memset(r_vendor, 0, 10);

    ret = open_padev(&pa_dev);
    if (ret)
        return errno;
    ret = get_vendor_padev(&pa_dev, r_vendor);
    if (ret)
        return errno;
    ret = close_padev(&pa_dev);
    if (ret)
        return errno;
    ret = strncmp(vendor, (char *)r_vendor, 3);
    if (ret)
        return -1;
    else
        return 0;
}
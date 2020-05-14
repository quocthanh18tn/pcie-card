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

int sysfs_write(const char *path, char *value, int wlen)
{
	int fd, ret;
	fd = open(path, O_WRONLY);
	if(fd < 0){
		return -errno;
	}
	ret = write(fd, value, wlen);
	close(fd);
	return ret;
}

int sysfs_read(const char *path, char *value, int max_len)
{
	int fd, ret;
	fd = open(path, O_RDONLY);
	if(fd < 0){
		return -errno;
	}
	ret = read(fd, value, max_len);
	close(fd);
	return ret;
}
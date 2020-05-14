#ifndef __RRU_LOCAL_H__
#define __RRU_LOCAL_H__

int sysfs_write(const char *path, char *value, int len);
int sysfs_read(const char *path, char *value, int max_len);

#endif
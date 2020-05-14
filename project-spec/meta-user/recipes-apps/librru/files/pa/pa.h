/*
 * libpa.h. - library for PA
 * Quan Vu <quanvh9@viettel.com.vn>
 * Version 1.0
 */

#ifndef __PA_H__
#define __PA_H__

#include <stdint.h>
#include <linux/types.h>
#include "../rrudef.h"
#include "../librru.h"
#include "../rrulocal.h"

int open_padev(pa_device *pa_dev);
int close_padev(pa_device *pa_dev);

int get_vendor_padev(pa_device *pa_dev, uint8_t *buf);
int get_date_padev(pa_device *pa_dev, uint8_t *buf);

int start_padev(pa_device *pa_dev);
int get_bias_status_padev(pa_device *pa_dev, uint8_t *buf);
int get_temp_padev(pa_device *pa_dev, float *buf);
int get_volt_padev(pa_device *pa_dev, float *buf);
int get_curr_padev(pa_device *pa_dev, float *buf);

int get_pa_info(pa_info *info);
int get_pa_status(pa_status *status);
int pa_selftest(char *vendor);
#endif

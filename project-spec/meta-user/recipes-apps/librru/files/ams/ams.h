#ifndef __AMS_H__
#define __AMS_H__

#include "../librru.h"
#include "../rrudef.h"
#include "../rrulocal.h"

#define XADC_DEV_PATH 		"/sys/bus/platform/devices/ffa50000.ams/iio:device0/" 
#define TEMP_OFFSET_NAME 	"in_temp0_ps_temp_offset"
#define TEMP_RAW_NAME		"in_temp0_ps_temp_raw"
#define TEMP_SCALE_NAME		"in_temp0_ps_temp_scale"
#define VCCINT_RAW_NAME		"in_voltage0_vccint_raw"
#define VCCINT_SCALE_NAME	"in_voltage0_vccint_scale"
#define VCCAUX_RAW_NAME		"in_voltage1_vccaux_raw"
#define VCCAUX_SCALE_NAME 	"in_voltage1_vccaux_scale"
#define VCCBRAM_RAW_NAME	"in_voltage2_vccbram_raw"
#define VCCBRAM_SCALE_NAME	"in_voltage2_vccbram_scale"
#define VCCPINT_RAW_NAME 	"in_voltage3_vccpint_raw"
#define VCCPINT_SCALE_NAME	"in_voltage3_vccpint_scale"
#define VCCPAUX_RAW_NAME	"in_voltage4_vccpaux_raw"
#define VCCPAUX_SCALE_NAME	"in_voltage4_vccpaux_scale"
#define VCCODDR_RAW_NAME	"in_voltage5_vccoddr_raw"
#define VCCODDR_SCALE_NAME	"in_voltage5_vccoddr_scale"
#define VREFP_RAW_NAME		"in_voltage6_vrefp_raw"
#define VREFP_SCALE_NAME	"in_voltage6_vrefp_scale"
#define VREFN_RAW_NAME		"in_voltage7_vrefn_raw"
#define VREFN_SCALE_NAME	"in_voltage7_vrefn_scale"


#endif
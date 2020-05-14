#ifndef __RRU_LIB_H__
#define __RRU_LIB_H__

#include <termios.h>
#include <rrulog.h>
#include <stdint.h>

#define RRU_8T8R_EVT1 	0

#define NUMBER_RF_CHANNELS 4

#define RRU_EEPROM 		"/sys/devices/platform/amba/ff020000.i2c/i2c-0/i2c-9/9-0054/eeprom"
#define RRU_CONFIG_PATH ""
#define RRU_HW_DATA 	"/sw_5g/running/rru/data/rru_hw.dat"
#define MODE_PATH       "/sw_5g/running/rru/cfg/mode.cfg"
#define BANDWIDTH_PATH  "/sw_5g/running/rru/cfg/bandwidth.cfg"
#define FREQUENCY_PATH  "/sw_5g/running/rru/cfg/frequency.cfg"
#define POWER_PATH      "/sw_5g/running/rru/cfg/power.cfg"

#define SISO 0
#define MIMO 1
#define LOOPBACK 2

typedef enum{
  STATE_OFF,
  STATE_ON,
} state_t;

typedef enum{
	TX_CHANNEL_1 = 0,
	TX_CHANNEL_2,
	RX_CHANNEL_1,
	RX_CHANNEL_2,
	ORX_CHANNEL_1,
	ORX_CHANNEL_2,
} channel_index_t;

typedef struct{
	float temp;		// Degree Celsius
	float tx_bias;	// Ampere
	float vcc;		// Volt
	float tx_power;	// Watt
	float rx_power;	// Watt
} sfp_info_t;

typedef struct{
	double rv_out;
	double tx_cpl_out;
	double pa_current;
	double rssi_rx;
	double fwd_out;
	double rx_cpl_out;
} rf_adc_info_t;

typedef struct{
	float ptx;
	float pr;
	float r;
	float vswr;
	float pin;
	float gain;
	float ps;
	float h;
	float pa_temp;
	float rssi;
	float rx_power;
	float pfwd;
	float reflect;
} rf_info_t;

/**
  * struct pa_info - contains the info about vendor
  * @vendor: name of the vendor
  * @date: the manufacture date
  */
/* pa struct */
typedef struct __pa_info{
	uint8_t vendor[10];
	uint8_t date[7];
} pa_info;

/**
  * struct pa_status - contain data read from pa
  * @bias_status: the status of pa (for details, read specification)
  * @current: current of 8 channel
  * @voltage: the supply voltage
  * @temp: the temperature of 8 channel
  */
typedef struct __pa_status{
	uint8_t bias_status;
	float temperature[4];
	float current[8];
	float voltage;
} pa_status;

/**
  * struct pa_device - describle the pa
  * @addr: pa i2c 7-bit address
  * @adapter_nr: bus number
  * @file: file descriptor of i2c bus device file
  * @info: pa_info struct
  * @status: pa_status struct
  */
typedef struct __pa_device{
	uint8_t addr;
	int file;
} pa_device;


/*Indicate SFP module is pluged or not*/
typedef enum{
	INSTALLED,
	UNINSTALLED
}EnCpriInstallStt;

/*Indicate fiber is pluged or not*/
typedef enum{
	PLUGED,
	UNPLUGED
}EnCpriPlugStt;

/*Indicate SFP module type: SFP/ESFP/QSFP*/
typedef enum{
	SFP,
	ESFP,
	QSFP
}EnTypeSFP;

/* RRU Configuration Info*/
typedef struct _StrRruRuntime{
	int mode;	
	long bandwidth;				// Hz
	unsigned long long freq;	// Hz
	float power;
} rru_runtime_config;

/* PA Characteristic */
typedef struct _StrCalibrateData{
    float dpd_calib;
    float temp;
    int gain;
    int digital_gain;
    float a; // a,b : conversion coefficient
    float b;
    int calibrated_flag;
    float res[3];
}Calib_pa_data;

typedef struct _StrCalibratePA{
    int calibrated_flag;
    Calib_pa_data data[NUMBER_RF_CHANNELS];
}Calib_pa;

typedef struct _StrCalibrateVSWR{
    float a; // a,b,c : conversion coefficient
    float b;
    float c;
    int calibrated_flag;
}Calib_vswr_data;

typedef struct _StrCalibrateVswrData{
    int calibrated_flag;
    Calib_vswr_data data[NUMBER_RF_CHANNELS];
}Calib_vswr;

/* Hardware specifics */
typedef struct _StrHWInfo{
// CONFIG_SYS_I2C_EEPROM_CCID // From Uboot
	char id[4];         /* 0x00 - 0x03 EEPROM Data Format Type */
	char major;         /* 0x04        Board revision, major */
	char minor;         /* 0x05        Board revision, minor */
	char trx_sn[22];    /* 0x06 - 0x1C TRX Serial Number*/
	char date[6];       /* 0x1D - 0x23 Build Date */
	char pa_sn[20];     /* 0x24 - 0x37 PA Serial Number */
	char dup_sn[20];    /* 0x38 - 0x4C Duplexer Serial Number */
	char mac_count;     /* 0x4D        Number of MAC addresses */
	char mac_flag;      /* 0x4E        MAC table flags */
	char mac[4][6];     /* 0x4F - 0x67 MAC addresses */
	char rru_sn[24];	/* 0x68 - 0x80 RRU Serial Number */
	int crc;            /* 0x81        CRC32 checksum */
	Calib_pa calibpa; // Viettel Defined
    Calib_vswr calibvswr;
} hw_info;


/********** OAM RRU ****************************************************/
struct iio_context * get_rru_iio_ctx(void);
int get_rf_device_name(int id, char* name);
double watts2dbm(double watts);
int get_gain_config(float power, float *dpd_calib, int *gain, int *dac, float *temperature);

// set/get frequency of ADRV9009
int get_trx_frequency(unsigned id,unsigned long long *freq);
int set_trx_frequency(unsigned id,unsigned long long freq);

// set/get bandwidth of ADRV9009
int get_trx_bandwidth(unsigned id, channel_index_t index, unsigned long long *freq);

// set/get frequency of RRU
int set_rru_frequency(unsigned long long freq);
int get_rru_frequency(unsigned long long *freq);

// set/get state of ADRV9009
int set_trx_state(unsigned id, channel_index_t index, state_t state);
int get_trx_state(unsigned id, channel_index_t index, state_t *state);

// set/get gain of ADRV9009
int set_trx_gain(unsigned id, channel_index_t index, float gain);
int get_trx_gain(unsigned id, channel_index_t index, float *gain);

int set_rru_bandwidth(int bandwidth);
int set_rru_power(float power);

int do_calibrate_vswr_close_port(int channel, float *a_value);
int do_calibrate_vswr_open_port(int channel, float a_value, Calib_vswr_data *data);
int do_calibrate_pa(int channel, float pOutExpect, float pOutMeasure, Calib_pa_data *data);

void set_gain_digital(int id, int gain);
unsigned int crc32(unsigned char *message, unsigned int length);

/*
* Function for getting clk status
* Return value: 
*   0 : Not supported
*	1 : Supported
*/
int is_vss_supported(void);

/*
* Function for getting clk status
* Return value: bit patten
* Bit 0 : If set indicate Clock from CTC is slow
* Bit 1 : If set indicate Clock from CTC is fast 
* Bit 2 : If set indicate Clock from CTC is fauty state
* Bit 3 : If set indicate Clock from CTC is invalid satate 
* Bit 4 : If set indicate RRU working in mode bypass checking clock
* Bit 5 - 31 : Reserved
*/
int rru_get_clock_status(void);

/********** OAM API ****************************************************/

int rru_open_device();
int rru_close_device();

// Dump all register
int rru_dump_registers(int id);
int rru_read_reg(int id, unsigned reg);
int rru_write_reg(int id, unsigned reg, unsigned value);

int rru_write_attr(int id, char *attr, char *value);
int rru_read_attr(int id, char *attr, char *value);

// Get all sfp info, return 0 on success, on error, a negative value is returned.
int get_sfp_info(int id, sfp_info_t *sfp_info);

// Get flash usage, return 0 on success, on error, a negative value is returned.
int get_flash_usage(unsigned long long *size, unsigned long long *used);

int get_memory_usage(unsigned long long *size, unsigned long long *used);

/*Get rf info, return 0 on success, on error, a negative value is returned.
* @index : 0 for TX1, RX1, FWD1...
*          1 for TX2, RX2, FWD2 
*/
int get_rf_info(int index, rf_info_t *rf_info);


// init rf path
int rru_rf_init(int id);

// reset rf path
int rru_rf_reset(int id);

//get led state, return 0 if on, return 1 if led off
int get_led_state(int led);

int set_led_state(int led, int val);

int set_led_blink(int led, int freq);

int get_gpio_state(int gpio);

int set_gpio_state(int gpio, int val);

// Get CPU temperature in Degree Celsius 
int get_cpu_temp(float *temp);

// Get PA info
int get_pa_info(pa_info *info);

// Get PA status
int get_pa_status(pa_status *status);

// PA Self test
int pa_selftest(char *vendor);

// Get RRU Hardware Version
int get_hardware_version(void);

// Get RRU configuration from file
int get_rru_runtime_config(rru_runtime_config *conf);

// Get RRU hardware infomation
int get_rru_hw_info(hw_info *hw);

// Set new RRU hardware infomation
int set_rru_hw_info(hw_info *hw);

int get_calib_data(Calib_pa_data *data, int ch);

//set CFR
int set_fpga_cfr(int channel, unsigned value);

// RRU Self Test
unsigned long long rru_self_test(void);

int set_core_value(uint64_t offset, long value);

unsigned get_core_value(uint64_t offset);

/**
 * Set RRU Board Infomation
 *  cmd_str = id
 *      program system id
 *  cmd_str = TRX
 *      program TRX serial number
 *  cmd_str = DUPLEXER
 *      program Duplexer serial number
 *  cmd_str = PA
 *      program PA serial number
 *  cmd_str = RRU"
 *      program system serial number
 *  cmd_str = date
 *      program date format YYMMDDhhmmss
 *  cmd_str = ports
 *      program the number of ports
 *  cmd_str = X
 *      program the MAC address for port X [X=0...3]"
 *      format: XX:XX:XX:XX:XX:XX"
 */
int do_set_new_hw_info(char *cmd_str, char *input);

void rru_safe_reboot(void);

void checklibrruversion(void);

#endif

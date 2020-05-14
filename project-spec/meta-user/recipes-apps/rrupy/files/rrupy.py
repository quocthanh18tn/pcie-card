#!/usr/bin/env python
from ctypes import Structure, c_char_p, c_uint, c_int, c_long, c_ulonglong, c_size_t, c_double, c_longlong, c_float, \
		c_uint64, c_uint8,c_ssize_t, c_char, c_void_p, c_bool, create_string_buffer, \
		POINTER as _POINTER, CDLL as _cdll, memmove as _memmove, byref as _byref
from sys import argv
lib = _cdll('librru.so.1.0')
class hw_state:
	OFF = 0
	ON = 1

class RF_CHANNEL:
	TX = 0
	RX = 1
	ORX = 2

class channel_index:
	RF_CHANNEL_1 = 1
	RF_CHANNEL_2 = 2
	RF_CHANNEL_3 = 3
	RF_CHANNEL_4 = 4
	RF_CHANNEL_5 = 5
	RF_CHANNEL_6 = 6
	RF_CHANNEL_7 = 7
	RF_CHANNEL_8 = 8

class sfp_device_t:
	sfp1 = 34
	sfp2 = 35

class LED:
	OFF = 0
	ON = 1

class EnCpriInstallStt:
	INSTALLED = 0
	UNINSTALLED = 1
class EnCpriPlugStt:
	PLUGED = 0
	UNPLUGED = 1

class EnTypeSFP:
	SFP = 0
	ESFP = 1
	QSFP = 2

class EnRfChannelStt:
	ON = 0
	OFF = 1

class sfp_info_t(Structure):
	_fields_ = [("temp", c_float),
				("tx_bias", c_float),
				("vcc", c_float),
				("tx_power", c_float),
				("rx_power", c_float)
				]

class rf_adc_info_t(Structure):
	_fields_ = [("rv_out", c_double),
				("tx_cpl_out", c_double),
				("pa_current", c_double),
				("rssi_rx", c_double),
				("fwd_out", c_double),
				("rx_cpl_out", c_double)
				]

class amc_temp_info_t(Structure):
	_fields_ = [("temp_local", c_float),
				("temp_d1", c_float),
				("temp_d2", c_float)
				]

class rf_info_t(Structure):
	_fields_ = [("ptx", c_float),
				("pr", c_float),
				("r", c_float),
				("vswr", c_float),
				("pin", c_float),
				("gain", c_float),
				("ps", c_float),
				("h", c_float),
				("pa_temp", c_float),
				("rssi", c_float),
				("rx_power", c_float),
				("pfwd", c_float),
				("reflect", c_float)
				]

class pa_info(Structure):
	_fields_ = [("vendor", c_uint8 * 10),
				("date", c_uint8 * 7)
				]

class pa_status(Structure):
	_fields_ = [("bias_status", c_uint8),
				("temperature", c_float * 4),
				("current", c_float * 8),
				("voltage", c_float)
				]		

class rru_power_info_t(Structure):
	_fields_ = [("volmon28", c_float),
				("volmon48", c_float)
				]

class rru_event_t(Structure):
	_fields_ = [("event", c_int)]

class ucd_vout_limit_t(Structure):
	_fields_ = [("ov_warning", c_float),
				("ov_fault", c_float),
				("uv_warning", c_float),
				("uv_fault", c_float)
				]

class rru_runtime_config(Structure):
	_fields_ = [("mode", c_int),
				("bandwidth", c_long),
				("freq", c_ulonglong),
				("power", c_float)
				]

class Calib_pa_data(Structure):
	_fields_ = [("dpd_calib", c_float),
				("temp", c_float),
				("gain", c_int),
				("digital_gain", c_int),
				("a", c_float),
				("b", c_float),
				("calibrated_flag", c_int),
				("res", c_float * 3)
				]

class Calib_pa(Structure):
	_fields_ = [("calibrated_flag", c_int),
				("data", Calib_pa_data * 4)
				]

class Calib_vswr_data(Structure):
	_fields_ = [("a", c_float),
				("b", c_float),
				("c", c_float),
				("calibrated_flag", c_int)
				]

class Calib_vswr(Structure):
	_fields_ = [("calibrated_flag", c_int),
				("data", Calib_vswr_data * 4)
				]

class hw_info(Structure):
	_fields_ = [("id", c_char * 4),
				("major", c_char),
				("minor", c_char),
				("trx_sn", c_char * 22),
				("date", c_char * 6),
				("pa_sn", c_char * 20),
				("dup_sn", c_char * 20),
				("mac_count", c_char),
				("mac_flag", c_char),
				("mac", (c_char * 6) * 4),
				("rru_sn", c_char * 24),
				("crc", c_int),
				("calibpa", Calib_pa),
				("calibvswr", Calib_vswr)
				]

watts2dbm = lib.watts2dbm
watts2dbm.argtypes = (c_double, )
watts2dbm.restype = c_double

get_gain_config = lib.get_gain_config
get_gain_config.argtypes = (c_float, _POINTER(c_float), _POINTER(c_int), _POINTER(c_int), _POINTER(c_float))
get_gain_config.restype = c_int

set_rru_power = lib.set_rru_power
set_rru_power.argtypes = (c_float, )
set_rru_power.restype = c_int

set_rru_freq = lib.set_rru_frequency
set_rru_freq.argtypes = (c_ulonglong, )
set_rru_freq.restype = c_int

get_rru_freq = lib.get_rru_frequency
get_rru_freq.argtypes = (_POINTER(c_ulonglong), )
get_rru_freq.restype = c_int

set_trx_frequency = lib.set_trx_frequency
set_trx_frequency.argtypes = (c_uint, c_ulonglong)
set_trx_frequency.restype = c_int

get_trx_frequency = lib.get_trx_frequency
get_trx_frequency.argtypes = (c_uint, _POINTER(c_ulonglong))
get_trx_frequency.restype = c_int

set_trx_gain = lib.set_trx_gain
set_trx_gain.argtypes = (c_uint, c_int, c_int)
set_trx_gain.restype = c_int

get_trx_gain = lib.get_trx_gain
get_trx_gain.argtypes = (c_uint, c_int, _POINTER(c_float))
get_trx_gain.restype = c_int

set_gain_digital = lib.set_gain_digital
set_gain_digital.argtypes = (c_int, c_int)

is_vss_supported = lib.is_vss_supported
is_vss_supported.restype = c_int

rru_open_device = lib.rru_open_device
rru_open_device.restype = c_int

rru_close_device = lib.rru_close_device
rru_close_device.restype = c_int

rru_dump_registers = lib.rru_dump_registers
rru_dump_registers.argtypes = (c_int, )
rru_dump_registers.restype = c_int

rru_write_reg = lib.rru_write_reg
rru_write_reg.argtypes = (c_uint, c_uint, c_int)
rru_write_reg.restype = c_int

rru_read_reg = lib.rru_read_reg
rru_read_reg.argtypes = (c_uint, c_uint)
rru_read_reg.restype = c_int

rru_write_attr = lib.rru_write_attr
rru_write_attr.argtypes = (c_uint, c_char_p, c_char_p)
rru_write_attr.restype = c_int

rru_read_attr = lib.rru_read_attr
rru_read_attr.argtypes = (c_uint, c_char_p, c_char_p)
rru_read_attr.restype = c_int

get_sfp_info = lib.get_sfp_info
get_sfp_info.argtypes = (c_int, _POINTER(sfp_info_t))
get_sfp_info.restype = c_int

get_flash_usage = lib.get_flash_usage
get_flash_usage.argtypes = (_POINTER(c_ulonglong), _POINTER(c_ulonglong))
get_flash_usage.restype = c_int

# get_rf_info = lib.get_rf_info
# get_rf_info.argtypes = (c_int, _POINTER(rf_info_t))
# get_rf_info.restype = c_int

rru_rf_init = lib.rru_rf_init
rru_rf_init.argtypes = (c_int, )
rru_rf_init.restype = c_int

# cpld_chip_reset = lib.cpld_chip_reset
# cpld_chip_reset.argtypes = (c_int, )
# cpld_chip_reset.restype = c_int

rru_rf_reset = lib.rru_rf_reset
rru_rf_reset.argtypes = (c_int, )
rru_rf_reset.restype = c_int

get_led_state = lib.get_led_state
get_led_state.argtypes = (c_int, )
get_led_state.restype = c_int

set_led_state = lib.set_led_state
set_led_state.argtypes = (c_int, c_int)
set_led_state.restype = c_int

set_led_blink = lib.set_led_blink
set_led_blink.argtypes = (c_int, c_int)
set_led_blink.restype = c_int

get_gpio_state = lib.get_gpio_state
get_gpio_state.argtypes = (c_int, )
get_gpio_state.restype = c_int

set_gpio_state = lib.set_gpio_state
set_gpio_state.argtypes = (c_int, c_int)
set_gpio_state.restype = c_int

get_trx_gain = lib.get_trx_gain
get_trx_gain.argtypes = (c_uint, c_int, _POINTER(c_float))
get_trx_gain.restype = c_int

set_trx_gain = lib.set_trx_gain
set_trx_gain.argtypes = (c_uint, c_int, c_float)
set_trx_gain.restype = c_int

get_cpu_temp = lib.get_cpu_temp
get_cpu_temp.argtypes = (_POINTER(c_float), )
get_cpu_temp.restype = c_int

xadc_get_temp = lib.xadc_get_temp
xadc_get_temp.argtypes = (_POINTER(c_float), )
xadc_get_temp.restype = c_int

get_pa_info = lib.get_pa_info
get_pa_info.argtypes = (_POINTER(pa_info), )
get_pa_info.restype = c_int

get_pa_status = lib.get_pa_status
get_pa_status.argtypes = (_POINTER(pa_status), )
get_pa_status.restype = c_int

get_hardware_version = lib.get_hardware_version
get_hardware_version.restype = c_int

get_rru_runtime_config = lib.get_rru_runtime_config
get_rru_runtime_config.argtypes = (_POINTER(rru_runtime_config), )
get_rru_runtime_config.restype = c_int

get_rru_hw_info = lib.get_rru_hw_info
get_rru_hw_info.argtypes = (_POINTER(hw_info), )
get_rru_hw_info.restype = c_int

set_rru_hw_info = lib.set_rru_hw_info
set_rru_hw_info.argtypes = (_POINTER(hw_info), )
set_rru_hw_info.restype = c_int

get_calib_data = lib.get_calib_data
get_calib_data.argtypes = (_POINTER(Calib_pa_data), c_int)
get_calib_data.restype = c_int

set_fpga_cfr = lib.set_fpga_cfr
set_fpga_cfr.argtypes = (c_int, c_int)
set_fpga_cfr.restype = c_int

rru_self_test = lib.rru_self_test
rru_self_test.restype = c_int

set_core_value = lib.set_core_value
set_core_value.argtypes = (c_uint64, c_long)
set_core_value.restype = c_int

get_core_value = lib.get_core_value
get_core_value.argtypes = (c_uint64, c_long)
get_core_value.restype = c_uint

do_set_new_hw_info = lib.do_set_new_hw_info
do_set_new_hw_info.argtypes = (c_char_p, c_char_p)
do_set_new_hw_info.restype = c_int

rru_safe_reboot = lib.rru_safe_reboot

checklibrruversion = lib.checklibrruversion

get_trx_bandwidth = lib.get_trx_bandwidth
get_trx_bandwidth.argtypes = (c_int, c_int, _POINTER(c_ulonglong))
get_trx_bandwidth.restype =  c_int

def info_pa():
	info = pa_info()
	ret = get_pa_info(_byref(info))
	if not ret:
		print('PA Vendor name: %c%c%c' % (info.vendor[0], info.vendor[1], info.vendor[2]))
		print('PA Version name: %c%c%c' % (info.vendor[3], info.vendor[4], info.vendor[5]))
		print('PA Vendor serial: %c%c%c' % (info.vendor[6], info.vendor[7], info.vendor[8]))
		print('Date: %c%c/%c%c/%c%c' % (info.date[0], info.date[1], info.date[2], info.date[3], info.date[4], info.date[5]))
	else:
		print('Error when reading PA infomation')

def status_pa():
	status = pa_status()
	ret = get_pa_status(_byref(status))
	if not ret:
		print('PA status: 0x%02x' % (status.bias_status))
		for x in range(8):
			print('PA current %d: %.3f' % (x+1, status.current[x]))
		print('PA voltage: %.3f' % status.voltage)
		for x in range(4):
			print('PA temp %d: %.3f' % (x+1, status.temperature[x]))
	else:
		print('Error when reading PA status')

def rru_cpu_get_info():
	temp = c_float()
	ret = get_cpu_temp(_byref(temp))
	print('CPU infomation')
	if not ret:
		print('CPU temperature: %.3f oC' % temp.value)
	else:
		print('Cannot get CPU info')


def int_conveter(value):
	##OK
	try:
		return int(value)
	except ValueError:
		return int(value,16)

def show_hw_info(rru):
	print('Hardware infomation')
	print('TRX SN: %s' % rru.trx_sn)
	print('PA SN: %s' % rru.pa_sn)
	print('DUP SN: %s' % rru.dup_sn)
	print('RRU SN: %s' % rru.rru_sn)
	print('Build date: 20%02x/%02x/%02x %02x:%02x:%02x ' 
		% (ord(rru.date[0]), ord(rru.date[1]), ord(rru.date[2]), ord(rru.date[3]) & 0x7F, ord(rru.date[4]), ord(rru.date[5])) 
		+ ("PM" if ord(rru.date[3]) & 0x80 else "AM"))
	max = 4 if int(rru.mac_count) >= 4 else int(rru.mac_count)
	for i in range(max):
		print('Eth%u: %02x:%02x:%02x:%02x:%02x:%02x:' 
			% (i, ord(rru.mac[i][0]), ord(rru.mac[i][1]), ord(rru.mac[i][2]), ord(rru.mac[i][3]), ord(rru.mac[i][4]), ord(rru.mac[i][5])))
	print('ID: %c%c%c%c' % (rru.id[0], rru.id[1], rru.id[2], rru.id[3]))

def sfp_print_info():
	##OK
	sfp_info = sfp_info_t()
	n = 4
	for RRU_ID_SFP in (5, 7, 9, 11):
		ret = get_sfp_info(RRU_ID_SFP, _byref(sfp_info))
		if ret < 0:
			print('SFP%d is not pluged' % (RRU_ID_SFP - n))
		else:
			print('\nSFP%d:' % (RRU_ID_SFP - n))
			print("Temperature: %.2f oC\n" % sfp_info.temp)
			print("TX bias: %.2f mA\n" % sfp_info.tx_bias )
			print("VCC: %.2f V\n" % sfp_info.vcc)
			print("TX power: %.2f mW\n" % sfp_info.tx_power)
			print("RX power: %.2f mW\n" % sfp_info.rx_power)
		n += 1

def rru_get_common_info():
	size = c_ulonglong()
	used = c_ulonglong()
	get_flash_usage(_byref(size), _byref(used))
	print('Flash size: %d, used %d' % (size.value, used.value))
	temp = c_float()
	xadc_get_temp(_byref(temp))
	print('xadc temperature %.3f oC' % temp.value)
def swap32(x):
	##OK
    return (((x << 24) & 0xFF000000) |
            ((x <<  8) & 0x00FF0000) |
            ((x >>  8) & 0x0000FF00) |
            ((x >> 24) & 0x000000FF))
def usage():
	print('''Usage:
	Reading reg		:rrutool read  reg chipID [reg]
	Writing reg		:rrutool write reg chipID [reg] [value]
	Register dump		:rrutool dump chipID
	Reseting		:rrutool reset chipID
Supported chipID:
	FAKEIIO			= 0
	LMK05028		= 1
	HMC7044			= 2
	HMC7043			= 3
	SFP_1_VENDOR		= 4
	SFP_1_DIAGNOSTIC	= 5
	SFP_2_VENDOR		= 6
	SFP_2_DIAGNOSTIC	= 7
	SFP_3_VENDOR		= 8
	SFP_3_DIAGNOSTIC	= 9
	SFP_4_VENDOR		= 10
	SFP_4_DIAGNOSTIC	= 11
	RRUGPIO			= 12
	IPCORE			= 13
	ADT7420			= 14
	LM73			= 15
	INA_12			= 16
	INA_28V			= 17
	INA_3V3_VCCO		= 18
	INA_1V8_VCCAUX		= 19
	INA_1V8_VCCO		= 20
	INA_1V2_VMGTAVTT	= 21
	INA_1V2_VCCO		= 22
	INA_0V9_VMGTAVCC	= 23
	INA_0V85_VCCPSINTLP	= 24
	INA_0V85_VCCINT		= 25
	INA_0V85_VCCPSINTFP	= 26
	INA_28V_PA		= 27
	ADRV9009_TRX12		= 28
	ADRV9009_TRX34		= 29
	ADRV9009_TRX56		= 30
	ADRV9009_TRX78		= 31
	ADRV9009_CALIB		= 32
	PCA9551			= 33
	RRU_ID_PA		= 34
Other commands:
	Get GPIO			: rrutool gpio get [pin]
	Set GPIO			: rrutool gpio set [pin] [state]
				See 8T8R pin list for pin value	
	Get Gain (for adrv9009)		: rrutool gain get [ID] [channel]
	Set Gain (for adrv9009)		: rrutool gain set [ID] [channel] [value]
	Get State (for adrv9009)	: rrutool state get [ID] [channel]
	Set State (for adrv9009)	: rrutool state set [ID] [channel] [state]
			Channel:
				TX_CHANNEL_1	: 0
				TX_CHANNEL_2	: 1
				RX_CHANNEL_1	: 2
				RX_CHANNEL_2	: 3
				ORX_CHANNEL_1	: 4
				ORX_CHANNEL_2	: 5
			State:
				OFF		: 0
				ON		: 1
	Get LED state			: rrutool led get [led]
	Set LED state			: rrutool led set [led] [state]
	Blink LED			: rrutool led blink [led] [fre]
			LED:
				CPRI 0 	: 0
				CPRI 1 	: 1
				CPRI 2 	: 2
				CPRI 3 	: 3
				ALARM	: 4
	Get frequency			: rrutool freq get [ID] 
	Set frequency			: rrutool freq set [ID] [value]
	Get rru frequency		: rrutool freq get rru
	Set rru frequency		: rrutool freq set rru [value]
	Get bandwidth			: rrutool bw [ID] [channel]
	Get hardware info		: rrutool hwinfo get
	Set hardware info		: rrutool hwinfo set [fields] [value]
		[fields] includes:
					T: for Serial number TRX 
					P: for Serial number PA
					D: for Serial number Duplexer
					n: for Serial number full
					R: for Serial number full
					d: for date BCD format YYMMDDhhmmss
					p: for MAC table size
					0 to 3: for "mac 0" through "mac 3" (format: "XX:XX:XX:XX:XX:XX")
	RRU selftest			: rrutool selftest
	Show sfp info			: rrutool sfpinfo
	Show system info		: rrutool sysinfo
	Show CPU info			: rrutool cpu
	Check rrutool version		: rrutool --version
	Check librru version		: rrutool -librru --version
	Help				: rrutool help
	''')
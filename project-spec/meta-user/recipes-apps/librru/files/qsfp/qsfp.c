#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <iio.h>
#include <math.h>
#include "../librru.h"
#include "../rrudef.h"
#include "../rrulocal.h"

/************* SFP registers definition *******************/
#define SFP_REG_CHIP_ID 0

#define SFP_DIAGNOSTIC_MONITORING_TYPE 92

#define SFP_REG_RX_PWR_4 56
#define SFP_REG_RX_PWR_3 60
#define SFP_REG_RX_PWR_2 64
#define SFP_REG_RX_PWR_1 68
#define SFP_REG_RX_PWR_0 72
#define SFP_REG_TX_I_SLOPE 76
#define SFP_REG_TX_I_OFFSET 78
#define SFP_REG_TX_PWR_SLOPE 80
#define SFP_REG_TX_PWR_OFFSET 82
#define SFP_REG_T_SLOPE 84
#define SFP_REG_T_OFFSET 86
#define SFP_REG_V_SLOPE 88
#define SFP_REG_V_OFFSET 90


#define SFP_REG_REAL_TIME_TEMP_MSB 96
#define SFP_REG_REAL_TIME_TEMP_LSB 97
#define SFP_REG_REAL_TIME_VCC_MSB 98
#define SFP_REG_REAL_TIME_VCC_LSB 99
#define SFP_REG_REAL_TIME_TXBIAS_MSB 100
#define SFP_REG_REAL_TIME_TXBIAS_LSB 101
#define SFP_REG_REAL_TIME_TXPOWER_MSB 102
#define SFP_REG_REAL_TIME_TXPOWER_LSB 103
#define SFP_REG_REAL_TIME_RXPOWER_MSB 104
#define SFP_REG_REAL_TIME_RXPOWER_LSB 105

#define SFP_REG_TEMP_H_ALARM_THRESHOLD  0
#define SFP_REG_TEMP_L_ALARM_THRESHOLD  2
#define SFP_REG_TEMP_H_WARN_THRESHOLD   4
#define SFP_REG_TEMP_L_WARN_THRESHOLD   6
#define SFP_REG_VOLT_H_ALARM_THRESHOLD  8
#define SFP_REG_VOLT_L_ALARM_THRESHOLD  10
#define SFP_REG_VOLT_H_WARN_THRESHOLD   12
#define SFP_REG_VOLT_L_WARN_THRESHOLD   14
#define SFP_REG_BIAS_H_ALARM_THERSHOLD  16
#define SFP_REG_BIAS_L_ALARM_THERSHOLD  18
#define SFP_REG_BIAS_H_WARN_THERSHOLD   20
#define SFP_REG_BIAS_L_WARN_THERSHOLD   22
#define SFP_REG_TXPWR_H_ALARM_THRESHOLD 24
#define SFP_REG_TXPWR_L_ALARM_THRESHOLD 26
#define SFP_REG_TXPWR_H_WARN_THRESHOLD  28
#define SFP_REG_TXPWR_L_WARN_THRESHOLD  30
#define SFP_REG_RXPWR_H_ALARM_THRESHOLD 32
#define SFP_REG_RXPWR_L_ALARM_THRESHOLD 34
#define SFP_REG_RXPWR_H_WARN_THRESHOLD  36
#define SFP_REG_RXPWR_L_WARN_THRESHOLD  38

#define SFP_REG_STATUS_CONTROL  110
#define SFP_REG_ALARM_FLAGS  112
#define SFP_REG_WARNING_FLAG  116

#define INVALID 0
#define INTERNALLY 1
#define EXTERNALLY 2

uint32_t rru_write_multi_regs(int id, uint32_t start_addr, uint32_t *value, int size){
    int ret;
    if(size==2){
        ret = rru_write_reg(id,start_addr,*value);
        if(ret<0) {
            return ret;
        }
        ret = rru_write_reg(id,start_addr+1,*(value+1));
        if(ret<0) {
            return ret;
        }
        return 0;
    }

    if(size==4){
        ret = rru_write_reg(id,start_addr,*value);
        if(ret<0) {
            return ret;
        }
        ret = rru_write_reg(id,start_addr+1,*(value+1));
        if(ret<0) {
            return ret;
        }
        ret = rru_write_reg(id,start_addr+2,*(value+2));
        if(ret<0) {
            return ret;
        }
        ret = rru_write_reg(id,start_addr+3,*(value+3));
        if(ret<0) {
            return ret;
        }
        return 0;
    }
}

uint32_t rru_read_multi_regs(int id, uint32_t start_addr, int size){
    if(size==2){
        uint8_t reg[2];
        reg[0] = rru_read_reg(id,start_addr);
        if(reg[0]<0) {
            return reg[0];
        }
        reg[1] = rru_read_reg(id,start_addr+1);
        if(reg[1]<0) {
            return reg[1];
        }
        return (uint32_t)((reg[0]&0xFF)<<8)+reg[1];
    }

    if(size==4){
        uint8_t reg[4];
        reg[0] = rru_read_reg(id,start_addr);
        if(reg[0]<0) {
            return reg[0];
        }
        reg[1] = rru_read_reg(id,start_addr+1);
        if(reg[1]<0) {
            return reg[1];
        }
        reg[2] = rru_read_reg(id,start_addr+2);
        if(reg[2]<0) {
            return reg[2];
        }
        reg[3] = rru_read_reg(id,start_addr+3);
        if(reg[3]<0) {
            return reg[3];
        }
        return (uint32_t)((reg[0]&0xFF)<<24)+(uint32_t)((reg[1]&0xFF)<<16)+(uint32_t)((reg[2]&0xFF)<<8)+reg[3];
    }
}

/* This function is for sfp vendor */
int set_calibrated_mode(int id, uint8_t calibrated_mode){
    uint32_t read_value = 0;
    int ret;
    read_value = rru_read_reg(id,SFP_DIAGNOSTIC_MONITORING_TYPE);
    if(read_value<0) {
        return read_value;
    }
    if(calibrated_mode==INTERNALLY){
        read_value |= 1UL << 5;
        read_value &= ~(1UL << 4);
        ret = rru_write_reg(id,SFP_DIAGNOSTIC_MONITORING_TYPE, read_value);
        if(ret<0) {
            return ret;
        }
    }
    else if(calibrated_mode==EXTERNALLY){
        read_value |= 1UL << 4;
        read_value &= ~(1UL << 5);
        ret = rru_write_reg(id,SFP_DIAGNOSTIC_MONITORING_TYPE, read_value);
        if(ret<0) {
            return ret;
        }
    }
    else{
        return -1;
    }
    return 0;
}

/* This function is for sfp vendor */
uint8_t get_calibrated_mode(int id){
    uint32_t read_value = 0;
    read_value = rru_read_reg(id,SFP_DIAGNOSTIC_MONITORING_TYPE);
    if(read_value<0) {
        return read_value;
    }
    else{
        uint8_t internally_calibrated = (read_value >> 5)&0x01;
        uint8_t externally_calibrated = (read_value >> 4)&0x01;
        if((internally_calibrated==1)&&(externally_calibrated==0)){
            return INTERNALLY;
        }
        else if((internally_calibrated==0)&&(externally_calibrated==1)){
            return EXTERNALLY;
        }
        else{
            return INVALID;
        }
    }
}

/* This function is for sfp diagnostic */
float get_temp(int id, uint8_t calibrated_mode){
    uint8_t high_byte;
    uint8_t low_byte;
    high_byte = rru_read_reg(id,SFP_REG_REAL_TIME_TEMP_MSB);
    if(high_byte<0) {
        return high_byte;
    }
    low_byte = rru_read_reg(id,SFP_REG_REAL_TIME_TEMP_LSB);
    if(low_byte<0) {
        return low_byte;
    }
    if(calibrated_mode==INTERNALLY){
        uint8_t sign = (high_byte>>7)&0x01;
        if(sign==0){
            high_byte &= ~(1UL << 7);
            return high_byte+low_byte/256.0;
        }
        if(sign==1){
            high_byte &= ~(1UL << 7);
            return high_byte-128+low_byte/256.0;
        }
    }
    else if(calibrated_mode==EXTERNALLY){
        float Tad;
        uint8_t sign = (high_byte>>7)&0x01;
        if(sign==0){
            high_byte &= ~(1UL << 7);
            Tad = high_byte+low_byte/256.0;
        }
        if(sign==1){
            high_byte &= ~(1UL << 7);
            Tad = high_byte-128+low_byte/256.0;
        }
        uint8_t slope_high_byte;
        uint8_t slope_low_byte;
        slope_high_byte = rru_read_reg(id,SFP_REG_T_SLOPE);
        if(slope_high_byte<0) {
            return slope_high_byte;
        }
        slope_low_byte = rru_read_reg(id,SFP_REG_T_SLOPE+1);
        if(slope_low_byte<0) {
            return slope_low_byte;
        }
        float Tslope = slope_high_byte+slope_low_byte/256.0;
        uint16_t Toffset = rru_read_multi_regs(id,SFP_REG_T_OFFSET,2);
        return (Tslope*Tad+Toffset)/256.0;
    }
    else{
        return -1;
    }
}

/* This function is for sfp diagnostic */
float get_supply_voltage(int id, uint8_t calibrated_mode){
    uint16_t vcc_ADC = rru_read_multi_regs(id,SFP_REG_REAL_TIME_VCC_MSB,2);
    if(vcc_ADC<0){
        return -1;
    }
    calibrated_mode = 1;
    if(calibrated_mode==INTERNALLY){
        return vcc_ADC*1.00/10000;
    }
    else if(calibrated_mode==EXTERNALLY){
        uint8_t slope_high_byte;
        uint8_t slope_low_byte;
        slope_high_byte = rru_read_reg(id,SFP_REG_T_SLOPE);
        if(slope_high_byte<0) {
            return slope_high_byte;
        }
        slope_low_byte = rru_read_reg(id,SFP_REG_T_SLOPE+1);
        if(slope_low_byte<0) {
            return slope_low_byte;
        }
        float Vslope = slope_high_byte+slope_low_byte/256.0;
        uint16_t Voffset = rru_read_multi_regs(id,SFP_REG_V_OFFSET,2);
        return (Vslope*vcc_ADC+Voffset)*1.00/10000;
    }
    else{
        return -1;
    }
}

/* This function is for sfp diagnostic */
float get_TX_bias_current(int id, uint8_t calibrated_mode){
    uint16_t TX_bias_current_ADC = rru_read_multi_regs(id,SFP_REG_REAL_TIME_TXBIAS_MSB,2);
    if(TX_bias_current_ADC<0){
        return -1;
    }
    if(calibrated_mode==INTERNALLY){
        return TX_bias_current_ADC*2.00/1000000;
    }
    else if(calibrated_mode==EXTERNALLY){
        uint8_t slope_high_byte;
        uint8_t slope_low_byte;
        slope_high_byte = rru_read_reg(id,SFP_REG_TX_I_SLOPE);
        if(slope_high_byte<0) {
            return slope_high_byte;
        }
        slope_low_byte = rru_read_reg(id,SFP_REG_TX_I_SLOPE+1);
        if(slope_low_byte<0) {
            return slope_low_byte;
        }
        float TX_Islope = slope_high_byte+slope_low_byte/256.0;
        uint16_t TX_Ioffset = rru_read_multi_regs(id,SFP_REG_TX_I_OFFSET,2);
        return (TX_Islope*TX_bias_current_ADC+TX_Ioffset)*2.00/1000000;
    }
    else{
        return -1;
    }
}

/* This function is for sfp diagnostic */
float get_TX_output_power(int id, uint8_t calibrated_mode){
    uint16_t TX_output_power_ADC = rru_read_multi_regs(id,SFP_REG_REAL_TIME_TXPOWER_MSB,2);
    if(TX_output_power_ADC<0){
        return -1;
    }
    if(calibrated_mode==INTERNALLY){
        return TX_output_power_ADC*0.1/1000000;
    }
    else if(calibrated_mode==EXTERNALLY){
        uint8_t slope_high_byte;
        uint8_t slope_low_byte;
        slope_high_byte = rru_read_reg(id,SFP_REG_TX_PWR_SLOPE);
        if(slope_high_byte<0) {
            return slope_high_byte;
        }
        slope_low_byte = rru_read_reg(id,SFP_REG_TX_PWR_SLOPE+1);
        if(slope_low_byte<0) {
            return slope_low_byte;
        }
        float TX_PWRslope = slope_high_byte+slope_low_byte/256.0;
        uint16_t TX_PWRoffset = rru_read_multi_regs(id,SFP_REG_TX_PWR_OFFSET,2);
        return (TX_PWRslope*TX_output_power_ADC+TX_PWRoffset)*0.1/1000000;
    }
    else{
        return -1;
    }
}

// Fake function to convert from 32 bit number to floating point number IEEE 754
// It must be developed when using EXTERNALLY to fulfil get_RX_input_power()
float Convert_to_IEEE_754_Floating_Point(uint32_t number){
    return (float)number;
}

/* This function is for sfp diagnostic */
float get_RX_input_power(int id, uint8_t calibrated_mode){
    uint16_t RX_input_power_ADC = rru_read_multi_regs(id,SFP_REG_REAL_TIME_RXPOWER_MSB,2);
    if(calibrated_mode==INTERNALLY){
        return RX_input_power_ADC*0.1/1000000;
    }
    else if(calibrated_mode==EXTERNALLY){
        uint32_t RX_PWR_4 = rru_read_multi_regs(id,SFP_REG_RX_PWR_4,4);
        uint32_t RX_PWR_3 = rru_read_multi_regs(id,SFP_REG_RX_PWR_3,4);
        uint32_t RX_PWR_2 = rru_read_multi_regs(id,SFP_REG_RX_PWR_2,4);
        uint32_t RX_PWR_1 = rru_read_multi_regs(id,SFP_REG_RX_PWR_1,4);
        uint32_t RX_PWR_0 = rru_read_multi_regs(id,SFP_REG_RX_PWR_0,4);
        float RX_PWR_4_float = Convert_to_IEEE_754_Floating_Point(RX_PWR_4);
        float RX_PWR_3_float = Convert_to_IEEE_754_Floating_Point(RX_PWR_3);
        float RX_PWR_2_float = Convert_to_IEEE_754_Floating_Point(RX_PWR_2);
        float RX_PWR_1_float = Convert_to_IEEE_754_Floating_Point(RX_PWR_1);
        float RX_PWR_0_float = Convert_to_IEEE_754_Floating_Point(RX_PWR_0);
        return RX_PWR_4_float*RX_input_power_ADC*RX_input_power_ADC*RX_input_power_ADC*RX_input_power_ADC
              +RX_PWR_3_float*RX_input_power_ADC*RX_input_power_ADC*RX_input_power_ADC
              +RX_PWR_2_float*RX_input_power_ADC*RX_input_power_ADC
              +RX_PWR_1_float*RX_input_power_ADC
              +RX_PWR_0_float;
    }
    else{
        return -1;
    }
}

int monitor_warning_flag(int id){
    uint8_t read_value;
    read_value = rru_read_reg(id,SFP_REG_WARNING_FLAG);
    if(read_value<0) {
        return read_value;
    }
    if((read_value>>7)&0x01 == 1){ printf("Temperature High Warning !\n");}
    if((read_value>>6)&0x01 == 1){ printf("Temperature Low Warning !\n");}
    if((read_value>>5)&0x01 == 1){ printf("Vcc High Warning !\n");}
    if((read_value>>4)&0x01 == 1){ printf("Vcc Low Warning !\n");}
    if((read_value>>3)&0x01 == 1){ printf("Tx Bias High Warning !\n");}
    if((read_value>>2)&0x01 == 1){ printf("Tx Bias Low Warning !\n");}
    if((read_value>>1)&0x01 == 1){ printf("Tx Power High Warning !\n");}
    if((read_value>>0)&0x01 == 1){ printf("Tx Power Low Warning !\n");}
    read_value = rru_read_reg(id,SFP_REG_WARNING_FLAG+1);
    if(read_value<0) {
        return read_value;
    }
    if((read_value>>7)&0x01 == 1){ printf("Rx Power High Warning !\n");}
    if((read_value>>6)&0x01 == 1){ printf("Rx Power Low Warning !\n");}
    return 0;
}

int monitor_alarm_flag(int id){
    int ret;
    uint8_t read_value;
    read_value = rru_read_reg(id,SFP_REG_ALARM_FLAGS);
    if(read_value<0) {
        return read_value;
    }
    if((read_value>>7)&0x01 == 1){ printf("Temperature High Alarm !\n");}
    if((read_value>>6)&0x01 == 1){ printf("Temperature Low Alarm !\n");}
    if((read_value>>5)&0x01 == 1){ printf("Vcc High Alarm !\n");}
    if((read_value>>4)&0x01 == 1){ printf("Vcc Low Alarm !\n");}
    if((read_value>>3)&0x01 == 1){ printf("Tx Bias High Alarm !\n");}
    if((read_value>>2)&0x01 == 1){ printf("Tx Bias Low Alarm !\n");}
    if((read_value>>1)&0x01 == 1){ printf("Tx Power High Alarm !\n");}
    if((read_value>>0)&0x01 == 1){ printf("Tx Power Low Alarm !\n");}
    read_value = rru_read_reg(id,SFP_REG_ALARM_FLAGS+1);
    if(read_value<0) {
        return read_value;
    }
    if((read_value>>7)&0x01 == 1){ printf("Rx Power High Alarm !\n");}
    if((read_value>>6)&0x01 == 1){ printf("Rx Power Low Alarm !\n");}
    return 0;
}

int get_sfp_info(int id, sfp_info_t *sfp_info){
    int ret;
    
    ret = get_temp(id, INTERNALLY); // Degree Celsius
    if (ret < 0) return ret;
    sfp_info->temp = ret;

    ret = get_TX_bias_current(id, INTERNALLY); // Ampere
    if (ret < 0) return ret;
    sfp_info->tx_bias = ret;

    ret = get_supply_voltage(id, INTERNALLY); // Volt
    if (ret < 0) return ret;
    sfp_info->vcc = ret;

    ret = get_TX_output_power(id, INTERNALLY); // Watt
    if (ret < 0) return ret;
    sfp_info->tx_power = ret;

    ret = get_RX_input_power(id, INTERNALLY); // Watt
    if (ret < 0) return ret;
    sfp_info->rx_power = ret;

    return 0;
}
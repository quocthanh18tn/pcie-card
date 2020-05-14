#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include "../librru.h"
#include "../rrudef.h"
#include <rrulocal.h>
#include <rrulog.h>
#include <rrulog.h>
#include "json/json.h"
#include <iio.h>
#include <stdlib.h>

int rru_rf_init(int id){
    LOG_WARN("Not supported %s", __func__);
    return 0;
}

int rru_rf_reset(int id){
    int ret = 0;
    switch(id){
        case RRU_ID_LMK05028 :
            break;
        case RRU_ID_HMC7044 :
            break;
        case RRU_ID_HMC7043 :
            break;
        case RRU_ID_IPCORE :
            break;
        case RRU_ID_ADT7420 :
            break;
        case RRU_ID_INA_12V :
            ret = rru_read_reg(RRU_ID_INA_12V, 0x00);
            if(ret < 0)
                break;
            ret = rru_write_reg(RRU_ID_INA_12V, 0x00, ret | 0x8000);
            break;
        case RRU_ID_INA_28V :
            ret = rru_read_reg(RRU_ID_INA_28V, 0x00);
            if(ret < 0)
                break;
            ret = rru_write_reg(RRU_ID_INA_28V, 0x00, ret | 0x8000);
            break;
        case RRU_ID_INA_3V3_VCCO :
            ret = rru_read_reg(RRU_ID_INA_3V3_VCCO, 0x00);
            if(ret < 0)
                break;
            ret = rru_write_reg(RRU_ID_INA_3V3_VCCO, 0x00, ret | 0x8000);
            break;
        case RRU_ID_INA_1V8_VCCAUX :
            ret = rru_read_reg(RRU_ID_INA_1V8_VCCAUX, 0x00);
            if(ret < 0)
                break;
            ret = rru_write_reg(RRU_ID_INA_1V8_VCCAUX, 0x00, ret | 0x8000);
            break;
        case RRU_ID_INA_1V8_VCCO :
            ret = rru_read_reg(RRU_ID_INA_1V8_VCCO, 0x00);
            if(ret < 0)
                break;
            ret = rru_write_reg(RRU_ID_INA_1V8_VCCO, 0x00, ret | 0x8000);
            break;
        case RRU_ID_INA_1V2_VMGTAVTT :
            ret = rru_read_reg(RRU_ID_INA_1V2_VMGTAVTT, 0x00);
            if(ret < 0)
                break;
            ret = rru_write_reg(RRU_ID_INA_1V2_VMGTAVTT, 0x00, ret | 0x8000);
            break;
        case RRU_ID_INA_1V2_VCCO :
            ret = rru_read_reg(RRU_ID_INA_1V2_VCCO, 0x00);
            if(ret < 0)
                break;
            ret = rru_write_reg(RRU_ID_INA_1V2_VCCO, 0x00, ret | 0x8000);
            break;
        case RRU_ID_INA_0V9_VMGTAVCC :
            ret = rru_read_reg(RRU_ID_INA_0V9_VMGTAVCC, 0x00);
            if(ret < 0)
                break;
            ret = rru_write_reg(RRU_ID_INA_0V9_VMGTAVCC, 0x00, ret | 0x8000);
            break;
        case RRU_ID_INA_0V85_VCCPSINTLP :
            ret = rru_read_reg(RRU_ID_INA_0V85_VCCPSINTLP, 0x00);
            if(ret < 0)
                break;
            ret = rru_write_reg(RRU_ID_INA_0V85_VCCPSINTLP, 0x00, ret | 0x8000);
            break;
        case RRU_ID_INA_0V85_VCCINT :
            ret = rru_read_reg(RRU_ID_INA_0V85_VCCINT, 0x00);
            if(ret < 0)
                break;
            ret = rru_write_reg(RRU_ID_INA_0V85_VCCINT, 0x00, ret | 0x8000);
            break;
        case RRU_ID_INA_0V85_VCCPSINTFP :
            ret = rru_read_reg(RRU_ID_INA_0V85_VCCPSINTFP, 0x00);
            if(ret < 0)
                break;
            ret = rru_write_reg(RRU_ID_INA_0V85_VCCPSINTFP, 0x00, ret | 0x8000);
            break;
        case RRU_ID_INA_28V_PA :
            ret = rru_read_reg(RRU_ID_INA_28V_PA, 0x00);
            if(ret < 0)
                break;
            ret = rru_write_reg(RRU_ID_INA_28V_PA, 0x00, ret | 0x8000);
            break;
        case RRU_ID_ADRV9009_TRX12 :
            break;
        case RRU_ID_ADRV9009_TRX34 :
            break;
        case RRU_ID_ADRV9009_TRX56 :
            break;
        case RRU_ID_ADRV9009_TRX78 :
            break;
        case RRU_ID_ADRV9009_CALIB :
            break;
        default:
            printf("Do not support RESET chip ID = %d\n", id);
            break;
    }
    return ret;
}

int set_trx_gain(unsigned id, channel_index_t index, float gain){
    char att_str[64];
    char gain_str[64];
    int ret;
    int channel;
    if(index == TX_CHANNEL_1)
        channel = 0;
    if(index == TX_CHANNEL_2)
        channel = 1;
    if(index == RX_CHANNEL_1)
        channel = 0;
    if(index == RX_CHANNEL_2)
        channel = 1;
    if(index == ORX_CHANNEL_1)
        channel = 2;
    if(index == ORX_CHANNEL_2)
        channel = 3;

    snprintf(gain_str, sizeof(gain_str), "%f", (float)gain);

    if((index == TX_CHANNEL_1) || (index == TX_CHANNEL_2))
        snprintf(att_str, sizeof(att_str), "out_voltage%d_hardwaregain", channel);

    if((index == RX_CHANNEL_1) || (index == RX_CHANNEL_2) || (index == ORX_CHANNEL_1) || (index == ORX_CHANNEL_2))
        snprintf(att_str, sizeof(att_str), "in_voltage%d_hardwaregain", channel);

    ret = rru_write_attr(id, att_str, gain_str);
    return ret;
}

int get_trx_gain(unsigned id, channel_index_t index, float *gain){
    char att_str[64]; 
    char gain_str[64];
    unsigned long tmp;
    int ret;
    int channel;
    if(index == TX_CHANNEL_1)
        channel = 0;
    if(index == TX_CHANNEL_2)
        channel = 1;
    if(index == RX_CHANNEL_1)
        channel = 0;
    if(index == RX_CHANNEL_2)
        channel = 1;
    if(index == ORX_CHANNEL_1)
        channel = 2;
    if(index == ORX_CHANNEL_2)
        channel = 3;

    if((index == TX_CHANNEL_1) || (index == TX_CHANNEL_2))
        snprintf(att_str, sizeof(att_str), "out_voltage%d_hardwaregain", channel);

    if((index == RX_CHANNEL_1) || (index == RX_CHANNEL_2) || (index == ORX_CHANNEL_1) || (index == ORX_CHANNEL_2))
        snprintf(att_str, sizeof(att_str), "in_voltage%d_hardwaregain", channel);

    ret = rru_read_attr(id, att_str, gain_str);
    if(ret < 0)
        return ret;

    *gain = strtof(gain_str, NULL);

    return ret;
}

int set_trx_state(unsigned id, channel_index_t index, state_t state){
    char att_str[64];
    int ret;
    int channel;
    if(index == TX_CHANNEL_1)
        channel = 0;
    if(index == TX_CHANNEL_2)
        channel = 1;
    if(index == RX_CHANNEL_1)
        channel = 0;
    if(index == RX_CHANNEL_2)
        channel = 1;
    if(index == ORX_CHANNEL_1)
        channel = 2;
    if(index == ORX_CHANNEL_2)
        channel = 3;

    if((index == TX_CHANNEL_1) || (index == TX_CHANNEL_2))
        snprintf(att_str, sizeof(att_str), "out_voltage%d_powerdown", channel);

    if((index == RX_CHANNEL_1) || (index == RX_CHANNEL_2) || (index == ORX_CHANNEL_1) || (index == ORX_CHANNEL_2))
        snprintf(att_str, sizeof(att_str), "in_voltage%d_powerdown", channel);

    if(state == STATE_OFF)
        ret = rru_write_attr(id, att_str, "1");
    else
        ret = rru_write_attr(id, att_str, "0");
    return ret;
}

int get_trx_state(unsigned id, channel_index_t index, state_t *state){
    char att_str[64], freq_str[64];
    unsigned long tmp;
    int ret;
    int channel;
    if(index == TX_CHANNEL_1)
        channel = 0;
    if(index == TX_CHANNEL_2)
        channel = 1;
    if(index == RX_CHANNEL_1)
        channel = 0;
    if(index == RX_CHANNEL_2)
        channel = 1;
    if(index == ORX_CHANNEL_1)
        channel = 2;
    if(index == ORX_CHANNEL_2)
        channel = 3;

    if((index == TX_CHANNEL_1) || (index == TX_CHANNEL_2))
        snprintf(att_str, sizeof(att_str), "out_voltage%d_powerdown", channel);

    if((index == RX_CHANNEL_1) || (index == RX_CHANNEL_2) || (index == ORX_CHANNEL_1) || (index == ORX_CHANNEL_2))
        snprintf(att_str, sizeof(att_str), "in_voltage%d_powerdown", channel);

    ret = rru_read_attr(id, att_str, freq_str);
    if(ret < 0)
        return ret;

    tmp = strtoul(freq_str, NULL, 10);
    if(tmp == 1)
        *state = STATE_OFF;
    if(tmp == 0)
        *state = STATE_ON;
    return ret;
}

int get_trx_frequency(unsigned id, unsigned long long *freq){
    char att_str[64], freq_str[64];
    int ret;
    sprintf(att_str, "out_altvoltage0_TRX_LO_frequency");
    ret = rru_read_attr(id, att_str, freq_str);
    *freq = strtoull(freq_str, NULL, 10);
    return ret;
}

int set_trx_frequency(unsigned id,unsigned long long freq){
    char att_str[64], freq_str[64];
    int ret;
    sprintf(att_str, "out_altvoltage0_TRX_LO_frequency");
    sprintf(freq_str, "%llu",freq);
    ret = rru_write_attr(id, att_str, freq_str);
    return ret;
}


static const char* bw_attr_st[] = {
    [TX_CHANNEL_1] = "out_voltage0_rf_bandwidth",
    [TX_CHANNEL_2] = "out_voltage1_rf_bandwidth",
    [RX_CHANNEL_1] = "in_voltage0_rf_bandwidth",
    [RX_CHANNEL_2] = "in_voltage1_rf_bandwidth",
    [ORX_CHANNEL_1] = "in_voltage2_rf_bandwidth",
    [ORX_CHANNEL_1] = "in_voltage3_rf_bandwidth",
};

int get_trx_bandwidth(unsigned id, channel_index_t index, unsigned long long *freq){
    char freq_str[64];
    int ret;
    ret = rru_read_attr(id, bw_attr_st[index], freq_str);
    *freq = strtoull(freq_str, NULL, 10);
    return ret;
}

int set_rru_frequency(unsigned long long freq){
    int ret;
    char str[128];
    
    if(freq < 75e6){
        printf("Frequency = %llu is too small\n", freq);
        return -1;
    }
    if(freq > 6000e6){
        printf("Frequency = %llu is too big\n", freq);
        return -1;
    }

    sprintf(str, "%llu", freq);
    save_string_to_file(FREQUENCY_PATH, str);
    
    printf("Setting TX = %llu Hz, RX = %llu Hz, FWD = %llu Hz\n", freq, freq, freq);

    set_trx_frequency(RRU_ID_ADRV9009_TRX12, freq); 
    set_trx_frequency(RRU_ID_ADRV9009_TRX34, freq); 
    set_trx_frequency(RRU_ID_ADRV9009_TRX56, freq); 
    set_trx_frequency(RRU_ID_ADRV9009_TRX78, freq); 
    set_trx_frequency(RRU_ID_ADRV9009_CALIB, freq); 

    return 0;
}

int get_rru_frequency(unsigned long long *freq){
    int ret;
    unsigned long long trx12_freq, trx34_freq, trx56_freq, trx78_freq, trx_calib_freq;

    ret = get_trx_frequency(RRU_ID_ADRV9009_TRX12, &trx12_freq); 
    if(ret < 0)
        return ret;
    else
        printf("TRX12 Frequency = %llu\n", trx12_freq);

    ret = get_trx_frequency(RRU_ID_ADRV9009_TRX34, &trx34_freq); 
    if(ret < 0)
        return ret;
    else
        printf("TRX34 Frequency = %llu\n", trx34_freq);

    ret = get_trx_frequency(RRU_ID_ADRV9009_TRX56, &trx56_freq); 
    if(ret < 0)
        return ret;
    else
        printf("TRX56 Frequency = %llu\n", trx56_freq);

    ret = get_trx_frequency(RRU_ID_ADRV9009_TRX78, &trx78_freq); 
    if(ret < 0)
        return ret;
    else
        printf("TRX78 Frequency = %llu\n", trx78_freq);

    ret = get_trx_frequency(RRU_ID_ADRV9009_CALIB, &trx_calib_freq); 
    if(ret < 0)
        return ret;
    else
        printf("TRX_CALIB Frequency = %llu\n", trx_calib_freq);
    
    if (trx12_freq==trx34_freq && trx34_freq==trx56_freq && trx56_freq==trx78_freq && trx78_freq==trx_calib_freq) {
        *freq = trx12_freq;
    }
    else{
        printf("Frequency in RF chips are not the same\n");
    }

    return 0;
}
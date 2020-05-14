#include "../librru.h"
#include "../rrudef.h"
#include "../rrulocal.h"

void set_gain_digital(int id, int gain){
    printf("Not supported %s\n", __func__);
}

int change_rru_power(float power){
    float dpd_calib[NUMBER_RF_CHANNELS], temperature[NUMBER_RF_CHANNELS];
    int gain[NUMBER_RF_CHANNELS], digital_gain[NUMBER_RF_CHANNELS], calibrated, i;
    calibrated = get_gain_config(power, &dpd_calib, &gain, &digital_gain, &temperature);
    for(i = 0; i < NUMBER_RF_CHANNELS ; i ++){
    	int step = (gain[i]+0.5f)*2-1;
    	char step_str[5];
    	sprintf(step_str,"%d",step);
    	//rru_write_attr(RRU_ID_TQM879026_1 + i, "in_steps0_raw", step_str);
    	//printf("Change gain TQM879026_%d [%d] = 0x%x, in compatible with step %dth\n", 1+i, RRU_ID_TQM879026_1 + i, gain[i], step); 
        //LOG_DEBUG("Change gain TQM879026_%d [%d] = 0x%x", 1+i, RRU_ID_TQM879026_1 + i, gain[i]);  
        set_gain_digital(i, digital_gain[i]);
    }
    return calibrated;
}

void save_calib_pa_data(Calib_pa_data *data, int channel){
    int ret;
    hw_info rru;
    ret = get_rru_hw_info(&rru);
    if(ret < 0){
        printf("Failed to get hardware infomation\n");
        return -1;
    }
  
    rru.calibpa.data[channel].dpd_calib = data->dpd_calib;
    rru.calibpa.data[channel].temp = data->temp;
    rru.calibpa.data[channel].gain = data->gain;
    rru.calibpa.data[channel].digital_gain = data->digital_gain;
    
    rru.calibpa.calibrated_flag = 0x12345678;
    
    set_rru_hw_info(&rru);
}

int pre_calibrate_pa(int channel){
    return 0;
}

int adjust_gain(int channel, float dpd_calib, float pOut, int gain_start , int *gain, int *digital_g){
	return 0;
}

int do_calibrate_pa(int channel, float pOutExpect, float pOutMeasure, Calib_pa_data *data){
    return 0;
}

void save_calib_vswr_data(Calib_vswr_data *data, int channel){
    int ret;
    hw_info rru;
    ret = get_rru_hw_info(&rru);
    if(ret < 0){
        printf("Failed to get hardware infomation\n");
        return -1;
    }
  
    rru.calibvswr.data[channel].a = data->a;
    rru.calibvswr.data[channel].b = data->b;
    rru.calibvswr.data[channel].c = data->c;
    
    rru.calibvswr.calibrated_flag = 0x12345678;

    set_rru_hw_info(&rru);
}

int do_calibrate_vswr_close_port(int channel, float *a_value){
	change_rru_power(10.0);
    printf("Not supported %s\n", __func__);
    return 0;
}

int do_calibrate_vswr_open_port(int channel, float a_value, Calib_vswr_data *data){
    printf("Not supported %s\n", __func__);
    return 0;
}
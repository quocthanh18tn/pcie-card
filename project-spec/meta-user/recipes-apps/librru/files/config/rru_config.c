#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "json/json.h"
#include "../librru.h"
#include "../rrudef.h"
#include <sys/fcntl.h>
#include "../rrulocal.h"


// All default is taken from 4G source, not yet refer to 5G specification
int DEFAULT_MODE = MIMO;
long DEFAULT_BANDWIDTH = 10000000;
unsigned long long DEFAULT_FREQUENCY = 1860000000;
float DEFAULT_POWER = 20.0;
float MAX_POWER = 40.0;

void str2lower(char *str, size_t size){
	int i = 0;
	while(i < size && str[i]){
		str[i] = (char) tolower((int)str[i]);
		i++;
	}
}

void save_string_to_file(const char *filename, char* str){
    FILE *file;
    file = fopen(filename, "w");
    fprintf(file, "%s", str);
    fclose(file);
}


int set_rru_power(float power){
    char str[128];
    if(power <= 0.0 || power > MAX_POWER)
        return -1;
    sprintf(str, "%2.0f", power);
    save_string_to_file(POWER_PATH, str);
    system("killall rrumon");
    return 0;
}

int set_rru_bandwidth(int bandwidth){
  char str[128];
  sprintf(str, "%lu", bandwidth);
  save_string_to_file(BANDWIDTH_PATH, str);
  //LOG_WARN("Change bw: %d, %s. RRU will be rebooted", bandwidth, str);
  //rru_safe_reboot();
  return 0;
}

int get_rru_runtime_config(rru_runtime_config *conf){
  FILE *file_mode;
  FILE *file_bandwidth;
  FILE *file_frequency;
  FILE *file_power;
  char buf[50];
  int ret = 0;

  if(access(MODE_PATH, R_OK) != -1) {
    // file exists
    file_mode = fopen(MODE_PATH, "r");
    memset(buf, '\0', 50);
    fgets(buf, 50, file_mode);
    conf->mode = MIMO;
    if(!strcmp(buf,"mimo\n")){
      conf->mode = MIMO;
    }
    else if(!strcmp(buf,"siso\n")){
      conf->mode = SISO;
    }
    else if(!strcmp(buf,"loopback\n")){
      conf->mode = LOOPBACK;
    }
    else if(!strcmp(buf,"MIMO\n")){
      conf->mode = MIMO;
    }
    else if(!strcmp(buf,"SISO\n")){
      conf->mode = SISO;
    }
    else if(!strcmp(buf,"LOOPBACK\n")){
      conf->mode = LOOPBACK;
    }
    fclose(file_mode);
  } else {
    // file doesn't exist
    LOG_DEBUG("%s not found\n", MODE_PATH);
    conf->mode = MIMO;
    ret -= 1;
  }  

  if(access(BANDWIDTH_PATH, R_OK) != -1) {
    // file exists
    file_bandwidth = fopen(BANDWIDTH_PATH, "r");
    memset(buf, '\0', 50);
    fgets(buf, 50, file_bandwidth);
    conf->bandwidth = strtol(buf, NULL, 0); 
    if((errno == EINVAL)||(errno == ERANGE)){
      LOG_DEBUG("Error when reading %s\n", BANDWIDTH_PATH);
      conf->bandwidth = DEFAULT_BANDWIDTH;
    }
    fclose(file_bandwidth);
  } else {
    // file doesn't exist
    LOG_DEBUG("%s not found\n", BANDWIDTH_PATH);
    conf->bandwidth = DEFAULT_BANDWIDTH;
    ret -= 1;
  }  

  if(access(FREQUENCY_PATH, R_OK) != -1) {
    // file exists
    file_frequency = fopen(FREQUENCY_PATH, "r");
    memset(buf, '\0', 50);
    fgets(buf, 50, file_frequency);
    conf->freq = strtoull(buf, NULL, 0); 
    if((errno == EINVAL)||(errno == ERANGE)){
      LOG_DEBUG("Error when converting %s to number at file = %s\n", buf, FREQUENCY_PATH);
      LOG_DEBUG("Using default config frequency as 1860Mhz\n");
      conf->freq = DEFAULT_FREQUENCY;
    }
    fclose(file_frequency);
  } else {
    // file doesn't exist
    LOG_DEBUG("%s not found\n", FREQUENCY_PATH);
    conf->freq = DEFAULT_FREQUENCY;
    ret -= 1;
  }

  if(access(POWER_PATH, R_OK) != -1) {
    // file exists
    file_power = fopen(POWER_PATH, "r");
    memset(buf, '\0', 50);
    fgets(buf, 50, file_power);
    conf->power = strtof(buf, NULL);
    if(errno == ERANGE){
      conf->power = DEFAULT_POWER;
    }
    if(conf->power > MAX_POWER){
      LOG_DEBUG("Configure power is out of range supported range. Configure to maximum power = 40w");
      conf->power = MAX_POWER;
    }
    fclose(file_power);
  } else {
    // file doesn't exist
    LOG_DEBUG("%s not found\n", POWER_PATH);
    conf->power = DEFAULT_POWER;
    ret -= 1;
  }

  return ret;
}

int get_default_gain_40w_info(int* gaintx){
    char *map, val[50], node[16], str[20], num = 0;
    size_t size;
    int len, parent, num_of_node, num_of_el, ret = 0, child_pos[1], i;
    jsmntok_t *jsmntok;

    jsmntok = parse_json("/config/gain.cfg", &map, &size, &len);

    if(find_string(jsmntok, map, size, "tqm879026_config", val) < 0){
        LOG_DEBUG( "Can not find token tqm879026_config\n");
        ret = 0;
        goto error;
    }
    if(!strcmp(val,"disabled")){
        ret = 0;
    }else if(!strcmp(val,"enabled")){
        ret = 1;
    }

    for(i = 0; i < NUMBER_RF_CHANNELS; i ++){
        sprintf(str, "tqm879026_%d", i + 1);
        get_param_info (jsmntok, map, len, str, &num_of_el, &num_of_node, &parent);
        get_node_array(jsmntok, parent + 1, len, child_pos);
        get_string(map, jsmntok + child_pos[0] + 1, val);
        gaintx[i] = strtol(val, NULL, 0);
        //LOG_DEBUG("gaintx[%d] = %s, 0x%x\n", i, val, gaintx[i]);
    }

error:
    free_json(map, size, jsmntok);
    return ret;
}

double watts2dbm(double watts){
    return 10 * (log10(watts) + 3);
}

int get_calib_data(Calib_pa_data *data, int ch){
    hw_info rru;
    int ret;
    ret = get_rru_hw_info(&rru);
    if(ret<0){
      return ret;
    }
    else{
      data->dpd_calib = rru.calibpa.data[ch].dpd_calib;
      data->temp =  rru.calibpa.data[ch].temp;
      data->gain =  rru.calibpa.data[ch].gain;
      data->digital_gain =  rru.calibpa.data[ch].digital_gain; 
      data->calibrated_flag =  rru.calibpa.data[ch].calibrated_flag;
      data->a =  rru.calibpa.data[ch].a;
      data->b =  rru.calibpa.data[ch].b;
      return ret;
    }
}

int get_gain_config(float power, float *dpd_calib, int *gain, int *digital_gain, float *temperature){
    int i, gain40w[NUMBER_RF_CHANNELS], ret, config_flag, calibreated[NUMBER_RF_CHANNELS];
    int all_calibrated=1;
    double dbm;
    Calib_pa_data data[NUMBER_RF_CHANNELS];

    config_flag = get_default_gain_40w_info(gain40w);
    for(i = 0; i < NUMBER_RF_CHANNELS; i ++){
        digital_gain[i] = 0;
        get_calib_data(&data[i], i);
        if(data[i].calibrated_flag == 0x12345678 && !config_flag){
          gain40w[i] = data[i].gain;
          digital_gain[i] = data[i].digital_gain;
          temperature[i] = data[i].temp;
          dpd_calib[i] = data[i].dpd_calib;
          calibreated[i] = 1;
        }
    }

    printf("data[0] from rru_hw = %d\n", data[0].gain);
    printf("data[1] from rru_hw = %d\n", data[1].gain);
    printf("data[2] from rru_hw = %d\n", data[2].gain);
    printf("data[3] from rru_hw = %d\n", data[3].gain);

    printf("gain40w[0] from gain.cfg = %d\n", gain40w[0]);
    printf("gain40w[1] from gain.cfg = %d\n", gain40w[1]);
    printf("gain40w[2] from gain.cfg = %d\n", gain40w[2]);
    printf("gain40w[3] from gain.cfg = %d\n", gain40w[3]);

    dbm = watts2dbm(power);
    if(dbm > 46.1){
        LOG_DEBUG("%0.2f (watts) = %0.2f (dbm) is out of range supported range. Configure to maximum power = 46 (dbm)\n", power, dbm);
        dbm = 46;
    }

    ret = (long) (2 * (46 - dbm) + 0.5);
    printf("ret dbm = %d\n", ret);
    for(i = 0 ; i < NUMBER_RF_CHANNELS ; i ++){
        gain[i] = gain40w[i] - ret;
        if(gain[i] < 0){
            gain[i] = 0;
            LOG_DEBUG("TX1: %0.2f (watts) = %0.2f (dbm) is out of range supported range. Configure to minimum power = %d (dbm)\n", power, dbm, 46 - gain40w[i]/2);
        }
    }

    printf("To get %.2f, in TRX0 we need gain = 0x%X\n", power, gain[0]);
    printf("To get %.2f, in TRX1 we need gain = 0x%X\n", power, gain[1]);
    printf("To get %.2f, in TRX2 we need gain = 0x%X\n", power, gain[2]);
    printf("To get %.2f, in TRX3 we need gain = 0x%X\n", power, gain[3]);

    for(i = 0; i < NUMBER_RF_CHANNELS; i ++){
      if(calibreated[i] == 0){
        all_calibrated = 0;
        LOG_DEBUG("RF channel %d is not calibrated\n", i);
        break;
      }
    }

    return all_calibrated;
}



#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ams.h"

char* concat(char *s1, char *s2);
int read_file_xadc_int(char *fn);
float read_file_xadc_float(char *fn);

char* concat(char *s1, char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);

    if (!result){
    	
    	return 0;
    }
    strcpy(result, s1);
    strcat(result, s2);
    result[strlen(s1)+strlen(s2)+1] = '\0';
    return result;
}

int xadc_get_temp(float *temp)
{
	int in_temp0_offset;
	int in_temp0_raw;
	float in_temp0_scale;

  char c[30];
  FILE *fptr;

  // Read attribute "in_temp0_ps_temp_offset"
  if ((fptr = fopen(concat(XADC_DEV_PATH, TEMP_OFFSET_NAME), "r")) == NULL)
  {
      printf("Error! opening file");
      return -1;         
  }
  // reads text until newline 
  fscanf(fptr,"%[^\n]", c);
  in_temp0_offset = strtol(c, NULL, 0);
  printf("Data from the file: %d\n", in_temp0_offset);
  fclose(fptr);

  // Read attribute "in_temp0_ps_temp_raw"
  if ((fptr = fopen(concat(XADC_DEV_PATH, TEMP_RAW_NAME), "r")) == NULL)
  {
      printf("Error! opening file");
      return -1;         
  }
  // reads text until newline 
  fscanf(fptr,"%[^\n]", c);
  in_temp0_raw = strtol(c, NULL, 0);
  printf("Data from the file: %d\n", in_temp0_raw);
  fclose(fptr);

  // Read attribute "in_temp0_ps_temp_scale"
  if ((fptr = fopen(concat(XADC_DEV_PATH, TEMP_SCALE_NAME), "r")) == NULL)
  {
      printf("Error! opening file");
      return -1;         
  }
  // reads text until newline 
  fscanf(fptr,"%[^\n]", c);
  in_temp0_scale = strtof(c, NULL);
  printf("Data from the file: %f\n", in_temp0_scale);
  fclose(fptr);

	*temp = ((in_temp0_raw + in_temp0_offset)/ 1000.0f) * in_temp0_scale ;

	return 0;
}

int get_cpu_temp(float *temp){
    int ret = xadc_get_temp(temp);
    return ret;
}

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include "../librru.h"
#include "../rrudef.h"
#include <rrulocal.h>
#include <rrulog.h>
#include <rrulog.h>
#include "json/json.h"
#include <librru.h>
#include <rrudef.h>
#include <rrulocal.h>
#include <iio.h>

int rru_get_clock_status(void){
    int ret = 0, rc;
    //rc = rru_read_reg(RRU_ID_AD9548, 0xd0c);
    //ret = (rc & 0x7) | ((~rc) & 0x8) ;
    //rc = rru_read_reg(RRU_ID_AD9548, 0xa10);
    //ret = ret | (rc << 4);
    return ret;
} 

int rru_get_clock_syn_state(void){
    int ret;
    //ret = rru_read_reg(RRU_ID_AD9548, 0xd0a);

    if (ret < 0) return ret;

    if (ret == 0x72){
        return 1;
    }

    return 0;
} 


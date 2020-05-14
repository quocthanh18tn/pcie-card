#include "../rrudef.h"
#include "../librru.h"
#include "../rrulocal.h"

void rru_prepare_read(int id){
    /* This function is to prepare some work before access IC.
    And it is used in rru_dump_registers() function */
}

int rru_get_register_range(int id){
    switch(id){
        case RRU_ID_FAKEIIO :
            return 100;
        case RRU_ID_LMK05028 :
            return 785;
        case RRU_ID_HMC7044 :
            return 339;
        case RRU_ID_HMC7043 :
            return 339;
        case RRU_ID_SFP_1_VENDOR :
            return 127;
        case RRU_ID_SFP_1_DIAGNOSTIC :
            return 255;
        case RRU_ID_SFP_2_VENDOR :
            return 127;
        case RRU_ID_SFP_2_DIAGNOSTIC :
            return 255;
        case RRU_ID_SFP_3_VENDOR :
            return 127;
        case RRU_ID_SFP_3_DIAGNOSTIC :
            return 255;
        case RRU_ID_SFP_4_VENDOR :
            return 127;
        case RRU_ID_SFP_4_DIAGNOSTIC :
            return 255;
        case RRU_ID_RRUGPIO :
        case RRU_ID_IPCORE :
        case RRU_ID_ADT7420 :
            return 47;
        case RRU_ID_LM73 :
            return 7;
        case RRU_ID_INA_12V :
            return 7;
        case RRU_ID_INA_28V :
            return 7;
        case RRU_ID_INA_3V3_VCCO :
            return 7;
        case RRU_ID_INA_1V8_VCCAUX :
            return 7;
        case RRU_ID_INA_1V8_VCCO :
            return 7;
        case RRU_ID_INA_1V2_VMGTAVTT :
            return 7;
        case RRU_ID_INA_1V2_VCCO :
            return 7;
        case RRU_ID_INA_0V9_VMGTAVCC :
            return 7;
        case RRU_ID_INA_0V85_VCCPSINTLP :
            return 7;
        case RRU_ID_INA_0V85_VCCINT :
            return 7;
        case RRU_ID_INA_0V85_VCCPSINTFP :
            return 7;
        case RRU_ID_INA_28V_PA :
            return 7;
        case RRU_ID_ADRV9009_TRX12 :
            return 16097;
        case RRU_ID_ADRV9009_TRX34 :
            return 16097;
        case RRU_ID_ADRV9009_TRX56 :
            return 16097;
        case RRU_ID_ADRV9009_TRX78 :
            return 16097;
        case RRU_ID_ADRV9009_CALIB :
            return 16097;
        case RRU_ID_PCA9551:
            return 6;
        default :
            return -1;
    }
}

int rru_dump_registers(int id){
    int ret;
    int err_count = 0, i, max;

    max = rru_get_register_range(id);
    if(max < 0){
        printf("Unsupported dump command\n");
        return 0;
    }   

    rru_prepare_read(id);

    for (i = 0; i <= max; i++ ){
        ret = rru_read_reg(id, i);
        if (ret < 0){
            err_count++;
        }else{
            printf("0x%08x : 0x%08x\n", i, ret);
        }
    }
    printf("Dumped %d register, %d error\n", (max+1), err_count);
    return 0;
}

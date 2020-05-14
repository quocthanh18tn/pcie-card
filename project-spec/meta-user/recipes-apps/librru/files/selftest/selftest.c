#include "../librru.h"
#include "../rrudef.h"
#include <sys/fcntl.h>
#include "../rrulocal.h"

int error_count = 0;
unsigned long long selftest_error_status = 0;

void setbit(unsigned long long *number, int n){
    *number |= 1ULL << n;
}

void clearbit(unsigned long long *number, int n){
    *number &= ~(1ULL <<n);    
}

int rru_self_test_by_id(unsigned id, unsigned reg){
	int backup_reg;
	int read_reg;
	int ret = 0;

	backup_reg = rru_read_reg(id, reg);
	if(backup_reg <0){
		error_count++;
		setbit(&selftest_error_status, id);
		return backup_reg;
	}
	ret = rru_write_reg(id, reg, 0xA0);
	if(ret){
		error_count++;
		setbit(&selftest_error_status, id);
		return ret;
	}
	read_reg = rru_read_reg(id, reg);
	if(read_reg < 0){
		error_count++;
		setbit(&selftest_error_status, id);
		return read_reg;
	}
	else{
		if(read_reg == 0xA0){
			clearbit(&selftest_error_status, id);
			rru_write_reg(id, reg, backup_reg);
			return 0;
		}
		else{
			error_count++;
			setbit(&selftest_error_status, id);
			return -1;
		}
	}
}

int print_sfp_vendor_name(int id){
    int ret;
    char reg[16];
    printf("SFP vendor: ");
    for(int i=0; i<16; i++){
        reg[i] = rru_read_reg(id,20+i);
        if((reg[i]<=0)||(reg[i]>127)) {
        	printf("\n");
            return -1;
        }
        printf("%c", reg[i]);
    }
    printf("\n");
    return 0;
}

unsigned long long rru_self_test(void){
	int ret = 0;
	printf("Self-testing ...\n");
	error_count = 0;
	selftest_error_status = 0;
	
	// RRU_ID_LMK05028
	ret = rru_self_test_by_id(RRU_ID_LMK05028, 0x302);
	if(ret)
		printf("LMK05028 selftest failed\n");
	else
		printf("LMK05028 selftest successfully\n");

	// RRU_ID_HMC7044
	ret = rru_self_test_by_id(RRU_ID_HMC7044, 0x08);
	if(ret)
		printf("RRU_ID_HMC7044 selftest failed\n");
	else
		printf("RRU_ID_HMC7044 selftest successfully\n");

	// RRU_ID_HMC7043
	ret = rru_self_test_by_id(RRU_ID_HMC7043, 0x08);
	if(ret)
		printf("RRU_ID_HMC7043 selftest failed\n");
	else
		printf("RRU_ID_HMC7043 selftest successfully\n");
	
	// RRU_ID_SFP_1_VENDOR
	ret = print_sfp_vendor_name(RRU_ID_SFP_1_VENDOR);
	if(ret){
		error_count++;
		setbit(&selftest_error_status, RRU_ID_SFP_1_VENDOR);
		printf("SFP_1 selftest failed\n");
	}
	else
		printf("SFP_1 selftest successfully\n");

	// RRU_ID_SFP_1_DIAGNOSTIC
	// Not check

	// RRU_ID_SFP_2_VENDOR
	ret = print_sfp_vendor_name(RRU_ID_SFP_2_VENDOR);
	if(ret){
		error_count++;
		setbit(&selftest_error_status, RRU_ID_SFP_2_VENDOR);
		printf("SFP_2 selftest failed\n");
	}
	else
		printf("SFP_2 selftest successfully\n");
	
	// RRU_ID_SFP_2_DIAGNOSTIC
	// Not check
	
	// RRU_ID_SFP_3_VENDOR
	ret = print_sfp_vendor_name(RRU_ID_SFP_3_VENDOR);
	if(ret){
		error_count++;
		setbit(&selftest_error_status, RRU_ID_SFP_3_VENDOR);
		printf("SFP_3 selftest failed\n");
	}
	else
		printf("SFP_3 selftest successfully\n");
	
	// RRU_ID_SFP_3_DIAGNOSTIC
	// Not check
	
	// RRU_ID_SFP_4_VENDOR
	ret = print_sfp_vendor_name(RRU_ID_SFP_4_VENDOR);
	if(ret){
		error_count++;
		setbit(&selftest_error_status, RRU_ID_SFP_4_VENDOR);
		printf("SFP_4 selftest failed\n");
	}
	else
		printf("SFP_4 selftest successfully\n");
	
	// RRU_ID_SFP_4_DIAGNOSTIC
	// Not check
	
	// RRU_ID_RRUGPIO
	// Not check
	
	// RRU_ID_ADT7420
	ret = rru_self_test_by_id(RRU_ID_ADT7420, 0x03);
	if(ret)
		printf("RRU_ID_ADT7420 selftest failed\n");
	else
		printf("RRU_ID_ADT7420 selftest successfully\n");
	
	// RRU_ID_LM73
	ret = rru_self_test_by_id(RRU_ID_LM73, 0x01);
	if(ret)
		printf("RRU_ID_LM73 selftest failed\n");
	else
		printf("RRU_ID_LM73 selftest successfully\n");
	
	// RRU_ID_INA_12V
	ret = rru_self_test_by_id(RRU_ID_INA_12V, 0x07);
	if(ret)
		printf("RRU_ID_INA_12V selftest failed\n");
	else
		printf("RRU_ID_INA_12V selftest successfully\n");
	
	// RRU_ID_INA_28V
	ret = rru_self_test_by_id(RRU_ID_INA_28V, 0x07);
	if(ret)
		printf("RRU_ID_INA_28V selftest failed\n");
	else
		printf("RRU_ID_INA_28V selftest successfully\n");
	
	// RRU_ID_INA_3V3_VCCO
	ret = rru_self_test_by_id(RRU_ID_INA_3V3_VCCO, 0x07);
	if(ret)
		printf("RRU_ID_INA_3V3_VCCO selftest failed\n");
	else
		printf("RRU_ID_INA_3V3_VCCO selftest successfully\n");
	
	// RRU_ID_INA_1V8_VCCAUX
	ret = rru_self_test_by_id(RRU_ID_INA_1V8_VCCAUX, 0x07);
	if(ret)
		printf("RRU_ID_INA_1V8_VCCAUX selftest failed\n");
	else
		printf("RRU_ID_INA_1V8_VCCAUX selftest successfully\n");
	
	// RRU_ID_INA_1V8_VCCO
	ret = rru_self_test_by_id(RRU_ID_INA_1V8_VCCO, 0x07);
	if(ret)
		printf("RRU_ID_INA_1V8_VCCO selftest failed\n");
	else
		printf("RRU_ID_INA_1V8_VCCO selftest successfully\n");
	
	// RRU_ID_INA_1V2_VMGTAVTT
	ret = rru_self_test_by_id(RRU_ID_INA_1V2_VMGTAVTT, 0x07);
	if(ret)
		printf("RRU_ID_INA_1V2_VMGTAVTT selftest failed\n");
	else
		printf("RRU_ID_INA_1V2_VMGTAVTT selftest successfully\n");
	
	// RRU_ID_INA_1V2_VCCO
	ret = rru_self_test_by_id(RRU_ID_INA_1V2_VCCO, 0x07);
	if(ret)
		printf("RRU_ID_INA_1V2_VCCO selftest failed\n");
	else
		printf("RRU_ID_INA_1V2_VCCO selftest successfully\n");
	
	// RRU_ID_INA_0V9_VMGTAVCC
	ret = rru_self_test_by_id(RRU_ID_INA_0V9_VMGTAVCC, 0x07);
	if(ret)
		printf("RRU_ID_INA_0V9_VMGTAVCC selftest failed\n");
	else
		printf("RRU_ID_INA_0V9_VMGTAVCC selftest successfully\n");
	
	// RRU_ID_INA_0V85_VCCPSINTLP
	ret = rru_self_test_by_id(RRU_ID_INA_0V85_VCCPSINTLP, 0x07);
	if(ret)
		printf("RRU_ID_INA_0V85_VCCPSINTLP selftest failed\n");
	else
		printf("RRU_ID_INA_0V85_VCCPSINTLP selftest successfully\n");
	
	// RRU_ID_INA_0V85_VCCINT
	ret = rru_self_test_by_id(RRU_ID_INA_0V85_VCCINT, 0x07);
	if(ret)
		printf("RRU_ID_INA_0V85_VCCINT selftest failed\n");
	else
		printf("RRU_ID_INA_0V85_VCCINT selftest successfully\n");
	
	// RRU_ID_INA_0V85_VCCPSINTFP
	ret = rru_self_test_by_id(RRU_ID_INA_0V85_VCCPSINTFP, 0x07);
	if(ret)
		printf("RRU_ID_INA_0V85_VCCPSINTFP selftest failed\n");
	else
		printf("RRU_ID_INA_0V85_VCCPSINTFP selftest successfully\n");
	
	// RRU_ID_INA_28V_PA
	ret = rru_self_test_by_id(RRU_ID_INA_28V_PA, 0x07);
	if(ret)
		printf("RRU_ID_INA_28V_PA selftest failed\n");
	else
		printf("RRU_ID_INA_28V_PA selftest successfully\n");
	
	// RRU_ID_ADRV9009_TRX12
	ret = rru_self_test_by_id(RRU_ID_ADRV9009_TRX12, 0x0A);
	if(ret)
		printf("RRU_ID_ADRV9009_TRX12 selftest failed\n");
	else
		printf("RRU_ID_ADRV9009_TRX12 selftest successfully\n");
	
	// RRU_ID_ADRV9009_TRX34
	ret = rru_self_test_by_id(RRU_ID_ADRV9009_TRX34, 0x0A);
	if(ret)
		printf("RRU_ID_ADRV9009_TRX34 selftest failed\n");
	else
		printf("RRU_ID_ADRV9009_TRX34 selftest successfully\n");
	
	// RRU_ID_ADRV9009_TRX56
	ret = rru_self_test_by_id(RRU_ID_ADRV9009_TRX56, 0x0A);
	if(ret)
		printf("RRU_ID_ADRV9009_TRX56 selftest failed\n");
	else
		printf("RRU_ID_ADRV9009_TRX56 selftest successfully\n");
	
	// RRU_ID_ADRV9009_TRX78
	ret = rru_self_test_by_id(RRU_ID_ADRV9009_TRX78, 0x0A);
	if(ret)
		printf("RRU_ID_ADRV9009_TRX78 selftest failed\n");
	else
		printf("RRU_ID_ADRV9009_TRX78 selftest successfully\n");
	
	// RRU_ID_ADRV9009_CALIB
	ret = rru_self_test_by_id(RRU_ID_ADRV9009_CALIB, 0x0A);
	if(ret)
		printf("RRU_ID_ADRV9009_CALIB selftest failed\n");
	else
		printf("RRU_ID_ADRV9009_CALIB selftest successfully\n");
	
	// RRU_ID_PCA9551
	ret = rru_self_test_by_id(RRU_ID_PCA9551, 0x02);
	if(ret)
		printf("RRU_ID_PCA9551 selftest failed\n");
	else
		printf("RRU_ID_PCA9551 selftest successfully\n");
	
	// RRU_ID_PA
	ret = rru_read_reg(RRU_ID_PA,0x0A);
	printf("Ret PA = %d\n", ret);
    if(ret < 0){
		error_count++;
		setbit(&selftest_error_status, RRU_ID_PA);
		printf("RRU_ID_PA selftest failed\n");
	}
	else
		printf("RRU_ID_PA selftest successfully\n");

	// PA
	ret = pa_selftest("DOT");
	if (ret){
		error_count++;
		printf("PA selftest failed\n");
	}
	else
		printf("PA selftest successfully\n");
	// Test EEPROM
	// Add later, test with struct hwinfo
	// Bit order of EEPROM selftest is larger than RRU_ID_PA by 1

	printf("selftest_error_status = %llu\n", selftest_error_status);
    return selftest_error_status;
}
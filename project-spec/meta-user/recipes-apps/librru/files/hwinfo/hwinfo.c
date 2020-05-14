#include "../rrudef.h"
#include "../librru.h"
#include "../rrulocal.h"

#include <sys/fcntl.h>
#include <byteswap.h>

unsigned int crc32(unsigned char *message, unsigned int length) {
	int i, j;
	unsigned int byte, crc, mask;

	i = 0;
	crc = 0xFFFFFFFF;
	do {
	  byte = message[i];            // Get next byte.
	  crc = crc ^ byte;
	  for (j = 7; j >= 0; j--) {    // Do eight times.
	  	mask = -(crc & 1);
	  	crc = (crc >> 1) ^ (0xEDB88320 & mask);
	  }
	  i = i + 1;
	}while(--length);
	return ~crc;
}

void update_eeprom_crc(hw_info *e)
{
	unsigned int crc, length;
	length = sizeof(hw_info) - 4;
	crc = crc32((unsigned char *)e, length);
	e->crc = bswap_32(crc);
}

char h2i(char p)
{
	if ((p >= '0') && (p <= '9'))
		return p - '0';

	if ((p >= 'A') && (p <= 'F'))
		return (p - 'A') + 10;

	if ((p >= 'a') && (p <= 'f'))
		return (p - 'a') + 10;

	return 0;
}

void set_manufacture_date(const char *string, hw_info *e)
{
	unsigned int i;

	if (strlen(string) != 12) {
		printf("Usage: mac date YYMMDDhhmmss\n");
		return;
	}

	for (i = 0; i < 6; i++)
		e->date[i] = h2i(string[2 * i]) << 4 | h2i(string[2 * i + 1]);

	update_eeprom_crc(e);
}

/**
 * set_mac_address - stores a MAC address into the EEPROM
 *
 * This function takes a pointer to MAC address string
 * (i.e->"XX:XX:XX:XX:XX:XX", where "XX" is a two-digit hex number) and
 * stores it in one of the MAC address fields of the EEPROM local copy.
 */
void set_manufacture_mac_address(unsigned int index, const char *string, hw_info *e)
{
	char *p = (char *) string;
	unsigned int i;

	if ((index >= 4) || !string) {
		printf("Usage: mac <n> XX:XX:XX:XX:XX:XX\n");
		return;
	}

	for (i = 0; *p && (i < 6); i++) {
		e->mac[index][i] = strtol(p, &p, 16);
		if (*p == ':')
			p++;
	}

	update_eeprom_crc(e);
}


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
 *      program date
 *  cmd_str = ports
 *      program the number of ports
 *  cmd_str = X
 *      program the MAC address for port X [X=0...3]"
 */

int do_set_new_hw_info(char *cmd_str, char *input)
{
	char cmd;
	int rc;
	hw_info *e, rru;
	rc = get_rru_hw_info(&rru);
	if(rc < 0){
		printf("Failed to get hardware infomation\n");
		return -1;
	}

	e = &rru;

	cmd = cmd_str[0];

	/* We know we have at least one parameter  */
	//printf("Classifying ...\n");
	switch (cmd) {
	case 'T':   /* serial number TRX */
		memset(e->trx_sn, 0, sizeof(e->trx_sn));
		strncpy((char *)e->trx_sn, input, sizeof(e->trx_sn) - 1);
		update_eeprom_crc(e);
		break;
	case 'P':   /* serial number PA */
		memset(e->pa_sn, 0, sizeof(e->pa_sn));
		strncpy((char *)e->pa_sn, input, sizeof(e->pa_sn) - 1);
		update_eeprom_crc(e);
		break;
	case 'D':   /* serial number Duplexer*/
		memset(e->dup_sn, 0, sizeof(e->dup_sn));
		strncpy((char *)e->dup_sn, input, sizeof(e->dup_sn) - 1);
		update_eeprom_crc(e);
		break;
	case 'n':   /* serial number full*/
		memset(e->rru_sn, 0, sizeof(e->rru_sn));
		strncpy((char *)e->rru_sn, input, sizeof(e->rru_sn) - 1);
		update_eeprom_crc(e);
		break;
	case 'R':   /* serial number full*/
		//printf("Detect RRU command\n");
		memset(e->rru_sn, 0, sizeof(e->rru_sn));
		strncpy((char *)e->rru_sn, input, sizeof(e->rru_sn) - 1);
		//printf("e->rru_sn %s\n", e->rru_sn);
		update_eeprom_crc(e);
		break;
	case 'd':   /* date BCD format YYMMDDhhmmss */
		set_manufacture_date(input, e);
		break;
	case 'p':   /* MAC table size */
		e->mac_count = strtol(input, NULL, 16);
		update_eeprom_crc(e);
		break;
	case '0' ... '3':   /* "mac 0" through "mac 3" */
		set_manufacture_mac_address(strtol(cmd_str, NULL, 10), input, e);
		break;
		default:
		return -1;
	}

	memcpy(e->id, "CCID", 4);
	update_eeprom_crc(e);

	return set_rru_hw_info(e);
}

int get_rru_hw_info(hw_info *hw){
	FILE *file;
	int ret;
	//printf("Do get_rru_hw_info\n");
	if(access(RRU_HW_DATA, R_OK) != -1) {
		// file exists
		file = fopen(RRU_HW_DATA, "r");
		if (file == NULL){
			LOG_DEBUG("Failed to open hardware info\n");
			return -1;
		}
	} else {
		// file doesn't exist
		LOG_ERROR("%s not found\n", RRU_HW_DATA);
		return -1;
	}

	ret = fread(hw, sizeof(hw_info), 1, file);
	if(ret != 1){
		LOG_DEBUG("Failed to open read %s, error = %d\n",RRU_HW_DATA, ret );
		fclose(file);
		return -1;
	}
	fclose(file);
	return ret;
}

int set_rru_hw_info(hw_info *hw){
	FILE *fp, *fd;
	int ret;
	//printf("Do set_rru_hw_info\n");

	if(access(RRU_HW_DATA, W_OK) != -1) {
		// file exists
		fd = fopen(RRU_HW_DATA, "wb");
		if (fd == NULL){
			LOG_DEBUG("Open %s is error!\n", RRU_HW_DATA);
			return -1;
		}
	} else {
		// file doesn't exist
		LOG_ERROR("%s not found\n", RRU_HW_DATA);
		return -1;
	}

	ret = fwrite(hw, sizeof(hw_info), 1, fd);
	if(ret != 1){
		LOG_DEBUG("Failed to write %s\n", RRU_HW_DATA);
		return -1;
	}else{
		fclose(fd);
	}
	
	// Write to EEPROM
	if(access(RRU_HW_DATA, W_OK) != -1) {
		// file exists
		fp = fopen(RRU_EEPROM, "wb");
		if (fp == NULL){
			printf("Open %s is error!\n", RRU_EEPROM);
			return -1;
		}
	} else {
		// file doesn't exist
		LOG_ERROR("%s not found\n", RRU_HW_DATA);
		return -1;
	}
	
	ret = fwrite(hw, sizeof(hw_info), 1, fp);
	if(ret != 1){
		printf("Failed to write %s\n", RRU_EEPROM);
		fclose(fp);
		return -1;
	}else
	fclose(fp);

	return 0;
}
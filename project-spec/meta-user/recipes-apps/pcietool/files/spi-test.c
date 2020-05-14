#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <stdarg.h>

#include <sys/stat.h>
#include <linux/spi/spidev.h>

#define LOG_FILE "/tmp/spi-test.log"
#define LOG_F(fmt, ...) fprintf(file_out, fmt, ##__VA_ARGS__)

enum app_input_structure{
	test_app_name = 0,
	test_dev_name,
	test_file_addr,
	test_minimum_args,
	test_dat_send = test_minimum_args,
};

static void help()
{
	printf(
	"Usage: spi-test DEV_NAME FILE_ADDR [SEND]\n"
	"DEV_NAME: name of your device\n"
	"FILE_ADDR: address to deivce driver in /dev\n"
	"[SEND]: series of bytes will be sent to bus interface\n"
	"Ex: ./spi-test ADRV9009 /dev/spidev3-0 0x57 0x01 0x02\n"
	" --> Test spi device ADRV9009 at /dev/spidev3-0 sending [0x57, 0x01, 0x02] \n"
	"          the driver will read back 3 bytes automatically\n"
	);
}

int main(int argc, char const *argv[])
{
	if(argc < test_minimum_args){
		help();
		return -1;
	}

	int write_index, write_len, check_index, check_len;
	int i, ret, test_mode;
	char *buf;
	unsigned char *write_buf, *check_buf;
	FILE *file_out = NULL;

	write_index = test_dat_send;
	check_index = -1;

	for(i = test_dat_send; i < argc; i++){
		if(argv[i][0] == '-'){
			if(strlen(argv[i]) < 2){
				fprintf(stderr, "Invalid opts: %s\n", argv[i]);
				ret = -1;
				goto EXIT_SPI_TEST;
			}
			switch(argv[i][1]){
				// case 'r':
				// read_index = i+1;
				// break;
				case 'c':
				check_index = i+1;
				break;
				case 'v':
				file_out = stdout;
				write_index++;
				break;
				default:
				fprintf(stderr, "Unsupport opts: %s\n", argv[i]);
				ret = -1;
				goto EXIT_SPI_TEST;
			}
		}
	}

	if(!file_out){
		file_out = fopen(LOG_FILE, "a");
		if(!file_out){
			file_out = stdout;
			LOG_F("WARN: Unable to create file log. Using stdout\n");
		}else{
			LOG_F("\n-----TEST %s------\n", argv[test_dev_name]);
		}
	}

	if(check_index < 0){
		write_len = argc - write_index;
		check_len = 0;
	}else{
		write_len = check_index - write_index - 1;
		check_len = argc - check_index;
	}


	write_buf = (write_len > 0? (unsigned char*)malloc(write_len * sizeof(unsigned char)):NULL);
	check_buf = (check_len > 0? (unsigned char*)malloc(check_len * sizeof(unsigned char)):NULL);

	if((!write_buf && write_len) || \
		(!check_buf && check_len)){
		LOG_F("ERROR: Unable to allocate buffer\n");
		ret = -3;
		goto RELEASE_MEM;
	}

	// printf("Data send: ");
	for(i = 0; i < write_len; i++){
		write_buf[i] = strtol(argv[i + write_index], &buf, 0);
		// printf("0x%.2X ", write_buf[i]);
		if (*buf || !*argv[i]) {
			LOG_F("ERROR: %s is not a number!\n", argv[i + write_index]);
			// printf("\n");
			ret = -2;
			goto RELEASE_MEM;
		}
	}
	// printf("\n");
	// printf("Sending length: %d, Checking length: %d\n", write_len, check_len);

	// printf("Check resp: ");
	for(i = 0; i < check_len; i++){
		check_buf[i] = strtol(argv[i + check_index], &buf, 0);
		// printf("0x%.2X ", check_buf[i]);
		if (*buf || !*argv[i]) {
			LOG_F("ERROR: %s is not a number!\n", argv[i + check_index]);
			// printf("\n");
			ret = -2;
			goto RELEASE_MEM;
		}
	}
	// printf("\n");

	int fd;
	fd = open(argv[test_file_addr], O_RDWR);
	if(fd < 0){
		LOG_F("ERROR: Unable to open device file %s\n", argv[test_file_addr]);
		ret = -4;
		goto RELEASE_MEM;
	}

 	struct spi_ioc_transfer xfer;
 	xfer.tx_buf = (unsigned long)write_buf;
 	xfer.rx_buf = (unsigned long)write_buf;
 	xfer.len = write_len;
 	xfer.speed_hz = 1000000;
 	xfer.delay_usecs = 0;
 	xfer.bits_per_word = 8;
 	xfer.cs_change = 0;

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);
    if (ret < 0){
		LOG_F("ERROR: Failed to send data");
		goto RELEASE_SPI;
    }else{
    	LOG_F("Data after transfer: ");
    	for(i = 0; i < write_len; i++){
    		LOG_F("0x%.2X ", write_buf[i]);
    	}
    	LOG_F("\n");
    }

	if(check_len){
		int j, found;
		found = 0;
		for(i = 0; i < check_len; i++){
			for(j = 0; j < write_len; j++){
				if(check_buf[i] == write_buf[j]){
					found++;
				}
			}
		}
		if(!found){
			LOG_F("ERROR: Checking response failed\n");
			ret = -5;
		}else{
			LOG_F("OK: Checking complete\n");
			ret = 0;
		}
	}

RELEASE_SPI:
	close(fd);
RELEASE_MEM:
	if(write_buf){
		free(write_buf);
	}
	if(check_buf){
		free(check_buf);
	}
EXIT_SPI_TEST:
	return ret;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <stdarg.h>
#include <errno.h>

#include <sys/stat.h>
#include <linux/i2c.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>

#define LOG_FILE "/tmp/i2c-test.log"
#define LOG_F(fmt, ...) fprintf(file_out, fmt, ##__VA_ARGS__)

enum app_input_structure{
	test_app_name = 0,
	test_dev_name,
	test_file_addr,
	test_dev_addr,
	test_minimum_args,
	test_dat_send = test_minimum_args,
};

static void help()
{
	printf(
	"Usage: i2c-test FILE_ADDR DEV_ADDR [SEND] [-r NUM-OF-READ] [-c CHECK_VALUE]\n"
	"FILE_ADDR: address to deivce driver in /dev\n"
	"DEV_ADDR: address in bus interface.\n"
	"[SEND]: series of bytes will be sent to bus interface\n"
	"[-r NUM-OF-READ]: Number of data will be read. SPI will ignore this parameter\n"
	"[-c CHECK_VALUE]: Series of bytes which are expected showing in response of device\n"
	"Ex: ./i2c-test /dev/i2c-0 0x57 0x01 0x02 -r 0x03\n"
	" --> Test i2c device /dev/i2c-0 at address 0x57 by sending [0x01, 0x02] then read back 3 bytes\n"
	);
}

int main(int argc, char const *argv[])
{
	if(argc < test_minimum_args){
		help();
		return -1;
	}

	int write_index, write_len, read_index, read_len, check_index, check_len;
	int i, ret, test_mode;
	unsigned int dev_address;
	char *buf;
	unsigned char *write_buf, *read_buf, *check_buf;
	FILE *file_out = NULL;

	write_index = test_dat_send;
	check_index = read_index = -1;

	for(i = test_dat_send; i < argc; i++){
		if(argv[i][0] == '-'){
			if(strlen(argv[i]) < 2){
				fprintf(stderr, "Invalid opts: %s\n", argv[i]);
				ret = -1;
				goto EXIT_I2C_TEST;
			}
			switch(argv[i][1]){
				case 'r':
				read_index = i+1;
				break;
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
				goto EXIT_I2C_TEST;
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

	dev_address = strtol(argv[test_dev_addr], &buf, 0);
	if (*buf || !*argv[test_dev_addr]) {
		LOG_F("ERROR: %s is not a number!\n", argv[test_dev_addr]);
		ret = -2;
		goto EXIT_I2C_TEST;
	}

	if(read_index < 0){
		if(check_index < 0){
			write_len = argc - write_index;
		}else{
			write_len = check_index - write_index - 1;
		}
		check_len = read_len = 0;
		if(check_index > 0){
			LOG_F("WARN: Skip checking without reading\n");
		}
	}else{
		write_len = read_index - write_index - 1;
		read_len = strtol(argv[read_index], &buf, 0);
		if (*buf || !*argv[read_index]) {
			LOG_F("ERROR: %s is not a number!\n", argv[read_index]);
			ret = -2;
			goto EXIT_I2C_TEST;
		}
		if(check_index > 0){
			check_len = argc - check_index;
		}else{
			check_len = 0;
		}
	}

	write_buf = (write_len > 0? (char*)malloc(write_len * sizeof(char)):NULL);
	read_buf = (read_len > 0? (char*)malloc(read_len * sizeof(char)):NULL);
	check_buf = (check_len > 0? (char*)malloc(check_len * sizeof(char)):NULL);

	if((!write_buf && write_len) || \
		(!read_buf && read_len) || \
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
			printf("\n");
			ret = -2;
			goto RELEASE_MEM;
		}
	}
	// printf("\n");
	// printf("Sending length: %d, Receiving length: %d, Checking length: %d\n", write_len, read_len, check_len);

	// printf("Check resp: ");

	for(i = 0; i < check_len; i++){
		check_buf[i] = strtol(argv[i + check_index], &buf, 0);
		// printf("0x%.2X ", write_buf[i]);
		if (*buf || !*argv[i]) {
			LOG_F("ERROR: %s is not a number!\n", argv[i + check_index]);
			// printf("\n");
			ret = -2;
			goto RELEASE_MEM;
		}
	}

	int fd;
	fd = open(argv[test_file_addr], O_RDWR);
	if(fd < 0){
		LOG_F("ERROR (%d): Unable to open device file %s\n", errno, argv[test_file_addr]);
		ret = -4;
		goto RELEASE_MEM;
	}
	ret = ioctl(fd, I2C_SLAVE_FORCE, dev_address);
	if (ret < 0) {
	    LOG_F("ERROR (%d): Failed to acquire bus access and/or talk to slave.\n", errno);
	    ret = -4;
	    goto RELEASE_I2C;
	}

	struct i2c_rdwr_ioctl_data i2c_data;
	struct i2c_msg msgs[2];
	// memset(msgs, 0, sizeof(msgs));

	i2c_data.msgs = msgs;
	i2c_data.nmsgs = 0;

	if(write_len){
		msgs[i2c_data.nmsgs].addr = dev_address;
		msgs[i2c_data.nmsgs].flags = 0;
		msgs[i2c_data.nmsgs].buf = write_buf;
		msgs[i2c_data.nmsgs].len = write_len;
		i2c_data.nmsgs++;
	}

	if(read_len){
		msgs[i2c_data.nmsgs].addr = dev_address;
		msgs[i2c_data.nmsgs].flags = I2C_M_RD;
		msgs[i2c_data.nmsgs].buf = read_buf;
		msgs[i2c_data.nmsgs].len = read_len;
		i2c_data.nmsgs++;
	}

	if(i2c_data.nmsgs){
		ret = ioctl(fd, I2C_RDWR, &i2c_data);
		if(ret < 0){
			LOG_F("ERROR (%d-%d): Failed to send messages\n", ret, errno);
			goto RELEASE_I2C;
		}else{
			LOG_F("OK: Access I2C device successfully!\n");
		}
		if(read_len){
			LOG_F(" Read back: ");
			for(i = 0; i < read_len; i++){
				LOG_F("0x%.2X ", read_buf[i]);
			}
			LOG_F("\n");
		}
	}else{
		LOG_F("WARN: No action for empty R/W buffer\n");
	}

	if(check_len){
		int j, found;
		found = 0;
		for(i = 0; i < check_len; i++){
			for(j = 0; j < read_len; j++){
				if(check_buf[i] == read_buf[j]){
					found++;
					break;
				}
				// LOG_F("INFO: Expect: 0x%X, Resp: 0x%X\n", check_buf[i], read_buf[i]);
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

RELEASE_I2C:
	close(fd);
RELEASE_MEM:
	if(write_buf){
		free(write_buf);
	}
	if(read_buf){
		free(read_buf);
	}
	if(check_buf){
		free(check_buf);
	}
EXIT_I2C_TEST:
	if(file_out != stdout && file_out != NULL){
		fclose(file_out);
	}
	return ret;
}


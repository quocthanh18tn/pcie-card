/*
 * (C) Copyright 2014, Viettel Group.
 *
 * Module: oslog.h
 *
 * Description:
 *
 * Author: Namnv24@viettel.com.vn
 *
 * Review: Dungpka@viettel.com.vn
 */

#ifndef RRU_LOG_H_
#define RRU_LOG_H_

#include <stdio.h>

#define DEBUG	 	0
#define INFO 		1
#define WARN  		2
#define ERROR 		3
#define FATAL 		4

#define PRINT_SCREEN    2
#define OUTPUT_FILE     1

int  rru_config_log(char *,unsigned int, unsigned int);
void rru_logger(int level, char *fname, int line_number,const char *format,...);

#define LOG_WARN(message,...) rru_logger(WARN,__FILE__,__LINE__,message,##__VA_ARGS__)
#define LOG_DEBUG(message,...) rru_logger(DEBUG,__FILE__,__LINE__,message,##__VA_ARGS__)
#define LOG_INFO(message,...) rru_logger(INFO,__FILE__,__LINE__,message,##__VA_ARGS__)
#define LOG_ERROR(message,...) rru_logger(ERROR,__FILE__,__LINE__,message,##__VA_ARGS__)
#define LOG_FATAL(message,...) rru_logger(FATAL,__FILE__,__LINE__,message,##__VA_ARGS__)
#endif /* RRU_LOG_H_ */

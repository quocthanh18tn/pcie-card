/*  i
  log.c  copyright 2016. Vettek
  fengli@viettelus.com 

*/
#include <stdio.h>
#include <time.h>
#include "rrulog.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#define MAX_PATH_LEN       80
#define RRU_LOG_FILE_SPACE_MAX 500000
#define MAX_ROTATE         2
#define MAX_MSG_SIZE_PER_LINE 600

static FILE          *fd;
static unsigned int  local_filter;
// mode: bit 0. log, bit 1 print to screen, bit 
static unsigned int  log_direction;
static char          name_path[MAX_PATH_LEN];
static unsigned int  total_space_written ;
static int           rotate;
static char          *level_string[] = {
    "DEBUG",
    "INFO ",
    "WARN ",
    "ERROR",
    "FATAL"
};
int rru_config_log(char * path, unsigned int filter, unsigned mode)
{ 
   fd = NULL;
   total_space_written = 0;
   rotate = MAX_ROTATE;
   local_filter = filter;
   log_direction = mode;
   strcpy(name_path,path); 
   if (log_direction & OUTPUT_FILE) {
     fd = fopen(path, "a");
     if (!fd) printf("error in open a file %s %d\n",__FUNCTION__,__LINE__);
     else {
           fseek(fd, 0, SEEK_END);
           total_space_written = ftell(fd);
           fseek(fd, 0 ,SEEK_SET);
     } 
   }
   return 0;
}
void  rru_close_log_file()
{
 if (fd) fclose(fd);
}
void rru_log_set_filter(unsigned int filter)
{
   local_filter = filter; 
}
void rru_log_setup_mode(unsigned int mode)
{ 
   log_direction = mode;
}

void rru_rotate_log(void)
{
   char file_name[MAX_PATH_LEN],old_name[MAX_PATH_LEN], new_name[MAX_PATH_LEN];
   int  i;
   sprintf(file_name, "%s", name_path);
   if (fd) {
	if (rotate > 0) {
        	for ( i = rotate - 1; i > 0; i-- )
		{
           		sprintf(old_name, "%s.%d", name_path, i-1);
           		sprintf(new_name, "%s.%d", name_path, i  );
           		rename(old_name, new_name); 
     		}
		sprintf(new_name,"%s.%d",name_path, 0);
		fflush(fd);
		fclose(fd);
		rename(file_name, new_name);
   	} else {
    		 fflush(fd);
     		 fclose(fd); 
   	}	
   fd = fopen(file_name, "a");
   if (!fd) printf("error in open a file %s %d\n",__FUNCTION__,__LINE__);
  } else { // ed == null
    fd = fopen(file_name,"a");
    if (fd) {
      fseek(fd, 0L, SEEK_END);
      total_space_written = ftell(fd);
    } else {
      printf("error in open a file %s %d\n",__FUNCTION__,__LINE__);
    } 
  }
      
}
int rru_write_log_message(char *msg)
{
   struct timeval  tv;
   struct tm      *tm;
   time_t ltime;
   unsigned int  write_size;

   gettimeofday(&tv, NULL);
   ltime = time(NULL);
   tm = localtime(&ltime);
   if (!fd) {
     fd = fopen(name_path, "a");
     if (!fd) printf("error in open a file %s %d\n",__FUNCTION__,__LINE__);
   }
   if (fd) {
	write_size = fprintf(fd, "%04d-%02d-%02d %02d:%02d:%02d %s\n",tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec,msg);
//        write_size = fprintf(fd, "%04d-%02d-%02d %02d:%02d:%02d:%06d %s\n",tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec,(int)tv.tv_usec,msg);
//  write_size = fprintf(fd, "%02d:%02d:%02d: %s\n",tm->tm_hour,tm->tm_min,tm->tm_sec,msg);


   total_space_written += write_size;   
   fflush(fd);
   }
   if (total_space_written > RRU_LOG_FILE_SPACE_MAX) {
       rru_rotate_log();
       total_space_written = 0;
   }
 return 0; 
}

void rru_logger(int level, char *fname, int line_number,const char *format,...)
{

  va_list argptr;
  char msg[MAX_MSG_SIZE_PER_LINE];
  int  len;
  if (level > FATAL) level = FATAL;
  if (log_direction & PRINT_SCREEN ) {
  }
  if (local_filter & (1<<level)) {
   
  	sprintf(msg,"%s %s #%d  ",level_string[level],fname,line_number);
  	len = strlen(msg);
  	va_start(argptr, format);
  	// 30 is for date and time.
  	vsnprintf(&msg[len], MAX_MSG_SIZE_PER_LINE-len-30, format, argptr);
  	va_end(argptr);
  	// maybe callback function here
       if (log_direction & PRINT_SCREEN ) {
          switch(level) {
           case DEBUG:  
                     printf("\33[34m[D] %s\33[0m\n",msg);
                     break;
           case INFO:
                     printf("[J] %s\n",msg);
                     break;
	   case WARN:	
 		    printf("\33[33m[W] %s \33[0m\n",msg);
                    break;
           default :
                    printf("\33[31m[E] %s \33[0m\n",msg);
                    break;
         };
       }
       if (log_direction & OUTPUT_FILE) {
 	   rru_write_log_message(msg);
       }
  }
} 

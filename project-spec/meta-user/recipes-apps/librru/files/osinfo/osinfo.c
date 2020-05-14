#include <librru.h>
#include <rrudef.h>
#include "../rrulocal.h"

int get_flash_usage(unsigned long long *size, unsigned long long *used){
    FILE *fp;
    char path[128], par[20];
    int found = 0;

    fp = popen("/bin/df", "r");

    if (fp == NULL) {
        LOG_DEBUG("Failed to run command\n" );
        return -1;
    }

    while (fgets(path, sizeof(path)-1, fp) != NULL) {
        if(strstr(path, "root") != NULL) {
            found = 1;
            break;
        }
    }

    pclose(fp);

    if (!found){
        return -1;
    }

    sscanf( path, "%19s %llu %llu", par, size, used);

    return 0;
}

int get_memory_usage(unsigned long long *size, unsigned long long *used){
    FILE * fp;
    char line[200];
    size_t len = 0;
    ssize_t read;
    char *detect1;
    char *detect2;
    const char needle1[10] = "MemTotal";
    const char needle2[10] = "MemFree";
    char field1[128];
    char field2[128];
    char unit1[10];
    char unit2[10];
    unsigned long long free_mem;

    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
        return 0;

    while(fgets(line, 200, (FILE*) fp)) {
        printf("%s", line);
        detect1 = strstr(line, needle1);
        if(detect1){
            detect1 = NULL;
            sscanf(line, "%s%lu%s", field1, size, unit1);
            continue;
        }
        detect2 = strstr(line, needle2);
        if(detect2){
            detect2 = NULL;
            sscanf(line, "%s%lu%s", field2, &free_mem, unit2);
            continue;
        }
    }
    
    // printf("MEMTOTAL = %lu kB\n", *size);
    // printf("MEMFREE = %lu kB\n", free_mem);
    // *used = *size - free_mem;
    // printf("MEMUSED = %lu kB", *used);

    *used = *size - free_mem;
    fclose(fp);
    return 0;
}
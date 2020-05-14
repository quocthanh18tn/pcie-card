#include <librru.h>
#include <rrudef.h>
#include <rrulocal.h>

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

#define CFR_THRESHOLD                           0x00000008
#define CFR_HARDCLIPPER_THRESHOLD               0x000001DC

#define PEAK_DETECT_1_ANT0  0x0000000C
#define PEAK_DETECT_2_ANT0  0x00000010
#define PEAK_DETECT_3_ANT0  0x00000014
#define PEAK_DETECT_4_ANT0  0x00000018
#define PEAK_DETECT_1_ANT1  0x0000800C
#define PEAK_DETECT_2_ANT1  0x00008010
#define PEAK_DETECT_3_ANT1  0x00008014
#define PEAK_DETECT_4_ANT1  0x00008018
#define PEAK_DETECT_1_ANT2  0x0001000C
#define PEAK_DETECT_2_ANT2  0x00010010
#define PEAK_DETECT_3_ANT2  0x00010014
#define PEAK_DETECT_4_ANT2  0x00010018
#define PEAK_DETECT_1_ANT3  0x0001800C
#define PEAK_DETECT_2_ANT3  0x00018010
#define PEAK_DETECT_3_ANT3  0x00018014
#define PEAK_DETECT_4_ANT3  0x00018018


int set_fpga_cfr(int channel, unsigned value){
    int rc = 0;
    //rru_write_reg(RRU_ID_CFR, PEAK_DETECT_1_ANT0 + channel * 0x8000, 0x28);
    //rru_write_reg(RRU_ID_CFR, PEAK_DETECT_2_ANT0 + channel * 0x8000, 0x28);
    //rru_write_reg(RRU_ID_CFR, PEAK_DETECT_3_ANT0 + channel * 0x8000, 0x28);
    //rru_write_reg(RRU_ID_CFR, PEAK_DETECT_4_ANT0 + channel * 0x8000, 0x28);
    //rc += rru_write_reg(RRU_ID_CFR, CFR_THRESHOLD + channel * 0x8000, value);
    //rc += rru_write_reg(RRU_ID_CFR, CFR_HARDCLIPPER_THRESHOLD + channel * 0x8000, value);
    return rc;
}

int set_core_value(uint64_t offset, long value){
    int fd, ret;
    uint64_t base;
    volatile uint8_t *mm;
    size_t page_size = ((size_t)getpagesize());
    uint64_t page_mask = ((uint64_t)(long)~(page_size - 1));

    fd = open("/dev/mem", O_RDWR | O_SYNC );
    if (fd < 0) {
        LOG_ERROR("open(/dev/mem) failed (%d)\n", errno);
        return -1;
    }

    base = offset & page_mask;
    offset &= ~page_mask;

    mm = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base);
    if (mm == MAP_FAILED) {
        LOG_ERROR("mmap64(0x%x@0x%lx) failed (%d)\n",
                page_size, base, errno);
        goto map_err;
    }

    *(volatile uint32_t *)(mm + offset) = value;

    munmap((void *)mm, page_size);
map_err:
    close(fd);

    return 0;
}

unsigned get_core_value(uint64_t offset){
    int fd;
    void *ptr;
    unsigned page_addr, page_offset, value;
    unsigned page_size=sysconf(_SC_PAGESIZE);

    fd=open("/dev/mem",O_RDONLY);
    if(fd<1) {
        return 0;
    }
    page_addr=(offset & ~(page_size-1));
    page_offset=offset-page_addr;

    ptr=mmap(NULL,page_size,PROT_READ,MAP_SHARED,fd,(offset & ~(page_size-1)));
    if((int)ptr==-1) {
        goto get_err;
    }

    value = *((unsigned *)(ptr+page_offset));
get_err:
    close(fd);
    return value;
}

int is_vss_supported(void){
    return 0;
}

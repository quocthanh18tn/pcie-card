#include <configs/platform-auto.h>
#define CONFIG_SYS_BOOTM_LEN 0xF000000

#define DFU_ALT_INFO_RAM \
                "dfu_ram_info=" \
        "setenv dfu_alt_info " \
        "image.ub ram $netstart 0x1e00000\0" \
        "dfu_ram=run dfu_ram_info && dfu 0 ram 0\0" \
        "thor_ram=run dfu_ram_info && thordown 0 ram 0\0"

#define DFU_ALT_INFO_MMC \
        "dfu_mmc_info=" \
        "set dfu_alt_info " \
        "${kernel_image} fat 0 1\\\\;" \
        "dfu_mmc=run dfu_mmc_info && dfu 0 mmc 0\0" \
        "thor_mmc=run dfu_mmc_info && thordown 0 mmc 0\0"

/* Extra U-Boot Env settings */
#define CONFIG_EXTRA_ENV_SETTINGS \
        SERIAL_MULTI \
        CONSOLE_ARG \
        PSSERIAL0 \
        "nc=setenv stdout nc;setenv stdin nc;\0" \
        "autoload=yes\0" \
        "sdbootdev=0\0" \
        "clobstart=0x10000000\0" \
        "netstart=0x10000000\0" \
        "dtbnetstart=0x23fff000\0" \
        "loadaddr=0x10000000\0" \
        "bootargs=earlycon clk_ignore_unused cpuidle.off=1 uio_pdrv_genirq.of_id=generic-uio mem=2G \0" \
        "bootsize=0x2800000\0" \
        "bootstart=0x0\0" \
        "boot_img=BOOT.BIN\0" \
        "load_boot=tftpboot ${clobstart} ${boot_img}\0" \
        "install_boot=sf probe 0 && sf erase ${bootstart} ${bootsize} && " \
                "sf write ${clobstart} ${bootstart} ${filesize}\0" \
        "bootenvsize=0x40000\0" \
        "bootenvstart=0x2800000\0" \
        "eraseenv=sf probe 0 && sf erase ${bootenvstart} ${bootenvsize}\0" \
        "jffs2size=0x9b00000\0" \ 
        "jffs2start=0x3c40000\0" \ 
        "jffs2_img=rootfs.jffs2\0" \ 
        "kernelsize=0x1400000\0" \
        "kernelstart=0x2840000\0" \
        "kernel_img=image.ub\0" \
        "load_jffs2=tftpboot ${clobstart} ${jffs2_img}\0" \ 
        "load_kernel=tftpboot ${clobstart} ${kernel_img}\0" \
        "install_jffs2=sf probe 0 && sf erase ${jffs2start} ${jffs2size} && " \ 
                "sf write ${clobstart} ${jffs2start} ${filesize}\0" \ 
        "install_kernel=sf probe 0 && sf erase ${kernelstart} ${kernelsize} && " \
                "sf write ${clobstart} ${kernelstart} ${filesize}\0" \
        "cp_kernel2ram=sf probe 0 && sf read ${netstart} ${kernelstart} ${kernelsize}\0" \
        "dtb_img=system.dtb\0" \
        "load_dtb=tftpboot ${clobstart} ${dtb_img}\0" \
        "fault=echo ${img} image size is greater than allocated place - partition ${img} is NOT UPDATED\0" \
        "test_crc=if imi ${clobstart}; then run test_img; else echo ${img} Bad CRC - ${img} is NOT UPDATED; fi\0" \
        "test_img=setenv var \"if test ${filesize} -gt ${psize}\\; then run fault\\; else run ${installcmd}\\; fi\"; run var; setenv var\0" \
        "netboot=tftpboot ${netstart} ${kernel_img} && bootm\0" \
        "default_bootcmd=run cp_kernel2ram && zynqmp mmio_write 0xFFD80050 0x0C 0x08 && zynqmp mmio_write 0xFFD80054 0xFFFFFFFF 0x00 && bootm ${netstart}\0" \
        "reset_mac=setenv ethaddr && saveenv && rrureset\0" \
""

/* BOOTCOMMAND */
#define CONFIG_BOOTCOMMAND      "run default_bootcmd"



/*Required for uartless designs */
#ifndef CONFIG_BAUDRATE
#define CONFIG_BAUDRATE 115200
#ifdef CONFIG_DEBUG_UART
#undef CONFIG_DEBUG_UART
#endif
#endif

/*Define CONFIG_ZYNQMP_EEPROM here and its necessaries in u-boot menuconfig if you had EEPROM memory. */
#ifdef CONFIG_ZYNQMP_EEPROM
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN  1
#define CONFIG_CMD_EEPROM
#define CONFIG_ZYNQ_EEPROM_BUS          5
#define CONFIG_ZYNQ_GEM_EEPROM_ADDR     0x54
#define CONFIG_ZYNQ_GEM_I2C_MAC_OFFSET  0x20
#endif


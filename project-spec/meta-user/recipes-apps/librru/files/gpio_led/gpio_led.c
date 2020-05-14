#include "../rrudef.h"
#include "../librru.h"
#include "../rrulocal.h"
#include <errno.h>
#include "gpiolib.h"

#include <unistd.h>

#define MAX_ADDRESS_LEN 128
#define MAX_LED_VALUE_LEN 10

enum led_id{
    led_cpri0 = 0,
    led_cpri1,
    led_cpri2,
    led_cpri3,
    led_alarm,
};

static const char *led_name[] = {
    [led_cpri0] = "pca955x:cpri0",
    [led_cpri1] = "pca955x:cpri1",
    [led_cpri2] = "pca955x:cpri2",
    [led_cpri3] = "pca955x:cpri3",
    [led_alarm] = "pca955x:alarm",
};

int set_gpio_state(int gpio, int val){
    int ret;
    /* We can use rrugpio IIO device for certain pin */
    //ret = rru_write_reg(RRU_ID_RRUGPIO, gpio, val);

    /* Or use sysfs for most of pin that are exported to userspace */
    /* By using gpiolib */
    ret = gpio_write(GET_GPIO_PIN(gpio), val);
    return ret;
}

int get_gpio_state(int gpio){
    int ret;
    /* We can use rrugpio IIO device for certain pin */
    // ret = rru_read_reg(RRU_ID_RRUGPIO, gpio);

    /* Or use sysfs for most of pin that are exported to userspace */
    /* By using gpiolib */
    ret = gpio_read(GET_GPIO_PIN(gpio));
    if(ret > 0)
        ret = 1;
    return ret;
}

int get_led_state(int led){
    char address_path[MAX_ADDRESS_LEN];
    char result_st[MAX_LED_VALUE_LEN];
    int ret, fd, result_i;

    ret = snprintf(address_path, MAX_ADDRESS_LEN, "/sys/class/leds/%s/brightness", led_name[led]);
    if(ret > 0){
        ret = sysfs_read(address_path, result_st, MAX_LED_VALUE_LEN);
        if(ret < 0){
            /* Do something to notif the error */
        }else{
            ret = strtol(result_st, NULL, 0);
        }
    }else{
        /* Unable to be here */
        ret = -ENOMEM;
    }
    return ret > 0;
}

int set_led_state(int led, int val){
    char address_path[MAX_ADDRESS_LEN];
    char val_st[MAX_LED_VALUE_LEN];
    int ret, fd;

    ret = snprintf(address_path, MAX_ADDRESS_LEN, "/sys/class/leds/%s/brightness", led_name[led]);
    if(ret < 0){
        return -ENOMEM;
    }
    ret = snprintf(val_st, MAX_LED_VALUE_LEN, "%d", val?255:0);
    if(ret < 0){
        return -ENOMEM;
    }
    ret = sysfs_write(address_path, val_st, ret);
    if(ret < 0){
        return ret;
    }
    return 0;
}

int set_led_blink(int led, int freq)
{
    char blink_path[MAX_ADDRESS_LEN];
    char val_st[MAX_LED_VALUE_LEN];
    int val_len;
    int ret, fd;

    val_len = snprintf(val_st, MAX_LED_VALUE_LEN, "%d", 500/freq);

    snprintf(blink_path, MAX_ADDRESS_LEN, "/sys/class/leds/%s/delay_on", led_name[led]);
    if(access(blink_path, F_OK) == -1){
        // printf("%s is not available. Trigger first\n", blink_path);
        char trigger_path[MAX_ADDRESS_LEN];
        snprintf(trigger_path, MAX_ADDRESS_LEN, "/sys/class/leds/%s/trigger", led_name[led]);
        ret = sysfs_write(trigger_path, "timer", 5);
        if(ret < 0){
            // fprintf(stderr, "Unable to write timer to %s\n", trigger_path);
            return ret;
        }
    }
    ret = sysfs_write(blink_path, val_st, val_len);
    if(ret < 0){
        // fprintf(stderr, "Unable to set %s to %s%s\n", val_st, blink_path);
        return ret;
    }
    snprintf(blink_path, MAX_ADDRESS_LEN, "/sys/class/leds/%s/delay_off", led_name[led]);
    ret = sysfs_write(blink_path, val_st, val_len);
    if(ret < 0){
        // fprintf(stderr, "Unable to set %s to %s%s\n", val_st, blink_path);
        return ret;
    }
    return 0;
}
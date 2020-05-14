#ifndef __RRUGPIO_H__
#define __RRUGPIO_H__

struct rrugpio_state {
	struct platform_device *pdev;
};

/* Functions declaration*/

void rru_set_gpio_state(int gpio, int state);
int rru_get_gpio_state(int gpio);

#endif

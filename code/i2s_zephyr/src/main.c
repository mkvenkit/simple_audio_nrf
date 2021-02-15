// main.c
#include <zephyr.h>

#include <drivers/uart.h>

#include <nrfx_i2s.h>

#include <drivers/gpio.h>

#include <stdio.h>
#include <stdlib.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(i2s_zephyr, LOG_LEVEL_INF);

#include "cmsis_nn_test.h"

// enable for I2S loopback test 
//#define LOOPBACK_TEST

#ifdef LOOPBACK_TEST
#include "loopback.h"
#endif

#include "cmsis_dsp_test.h"
#include "i2s_mic.h"

void main(void)
{
    LOG_INF("hello I2S...");
    printk("hello I2S...\n");

#ifdef LOOPBACK_TEST
    loopback_init();
#endif

    i2s_mic_init();

    i2s_mic_test();


    //struct device* dev = device_get_binding("GPIO_0");
    //gpio_pin_configure(dev, 26, GPIO_OUTPUT); 


    //struct device *dev = device_get_binding("UART_0");                                                                                                                                                                               
    //uart_poll_out(dev, 'A'); 

    //cmsis_dsp_test();

    cmsis_nn_test();
    
    // main loop 
    for(;;) {

#ifdef LOOPBACK_TEST
        //loopback_transfer();
        k_msleep(2000);
#endif 

    	//gpio_pin_toggle(dev, 26);

        k_msleep(200);
    }
}


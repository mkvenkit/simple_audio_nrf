// main.c
#include <zephyr.h>

#include <drivers/uart.h>

#include <nrfx_i2s.h>

#include <stdio.h>
#include <stdlib.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(i2s_zephyr, LOG_LEVEL_INF);


// enable for I2S loopback test 
#define LOOPBACK_TEST

#ifdef LOOPBACK_TEST
#include "loopback.h"
#endif

void main(void)
{
    LOG_INF("hello I2S...");

#ifdef LOOPBACK_TEST
    loopback_init();
#endif

    struct device *dev =                                                                                            
                 device_get_binding("UART_0");                                                        
                                                                                                                       
    uart_poll_out(dev, 'A'); 


    // main loop 
    for(;;) {

#ifdef LOOPBACK_TEST
        //loopback_transfer();
        k_msleep(2000);
#endif 

    }
}


// main.c
#include <zephyr.h>
#include <nrfx_i2s.h>

#include <stdio.h>
#include <stdlib.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(i2s_zephyr, LOG_LEVEL_INF);

#include "loopback.h"


void main(void)
{

    

    LOG_INF("hello I2S...");

    loopback_init();

    for(;;) {

        loopback_transfer();


        k_msleep(2000);
    }
}


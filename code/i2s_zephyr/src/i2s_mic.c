
#include <zephyr.h>
#include <nrfx_i2s.h>

#include "i2s_mic.h"


#include <logging/log.h>
LOG_MODULE_REGISTER(i2s_zephyr_mic, LOG_LEVEL_INF);

#define I2S_DATA_BLOCK_WORDS    512
static uint32_t m_buffer_rx[I2S_DATA_BLOCK_WORDS];


/* 
    data_handler

    Adapted from Nordic I2S loopback example.

*/
static void data_handler(nrfx_i2s_buffers_t const * p_released,
                         uint32_t                      status)
{
    // 'nrf_drv_i2s_next_buffers_set' is called directly from the handler
    // each time next buffers are requested, so data corruption is not
    // expected.
    //TODO assert(p_released);

    // When the handler is called after the transfer has been stopped
    // (no next buffers are needed, only the used buffers are to be
    // released), there is nothing to do.
    if (!(status & NRFX_I2S_STATUS_NEXT_BUFFERS_NEEDED))
    {
        return;
    }

    // First call of this handler occurs right after the transfer is started.
    // No data has been transferred yet at this point, so there is nothing to
    // check. Only the buffers for the next part of the transfer should be
    // provided.
    if (!p_released->p_rx_buffer)
    {
        nrfx_i2s_buffers_t const next_buffers = {
            .p_rx_buffer = m_buffer_rx,
            .p_tx_buffer = 0,
        };
        nrfx_i2s_next_buffers_set(&next_buffers);
    }
    else
    {
        uint32_t* data = p_released->p_rx_buffer;
        printk("data: %x, %x\n", data[0], data[1]);

        // The driver has just finished accessing the buffers pointed by
        // 'p_released'. They can be used for the next part of the transfer
        // that will be scheduled now.
        nrfx_i2s_next_buffers_set(p_released);
    }
}

ISR_DIRECT_DECLARE(i2s_isr_handler_mic) 
{
  nrfx_i2s_irq_handler();
  ISR_DIRECT_PM();

  return 1; 
}

void i2s_mic_init()
{
    nrfx_i2s_config_t config = NRFX_I2S_DEFAULT_CONFIG(26, 25, 255, 6, 7);
   
    // For SPH0645LM4H MEMS microphone, datasheet says: 
    // "the WS signal must be BCLK/64 and synchronized to the BCLK"
    // So set SCK to 32 MHz / 31 = 1.0322581 MHz and 
    // LRCLK to 1.0322581 MHz /64 = 16129.0328125 Hz
    config.sample_width = NRF_I2S_SWIDTH_24BIT_IN32BIT;
    config.channels  = NRF_I2S_CHANNELS_STEREO;
    config.mck_setup = NRF_I2S_MCK_32MDIV31;
    config.ratio     = NRF_I2S_RATIO_64X;

    IRQ_DIRECT_CONNECT(I2S0_IRQn, 0,i2s_isr_handler_mic, 0);

    int err_code = nrfx_i2s_init(&config, data_handler);

    LOG_INF("i2s_mic_init done...\n");
}

void i2s_mic_test()
{
    nrfx_i2s_buffers_t const initial_buffers = {
        .p_tx_buffer = 0,
        .p_rx_buffer = m_buffer_rx,
    };
    int err_code = nrfx_i2s_start(&initial_buffers, I2S_DATA_BLOCK_WORDS, 0);

}
#include "i2s_mic.h"



ISR_DIRECT_DECLARE(i2s_isr_handler) 
{
  nrfx_i2s_irq_handler();
  ISR_DIRECT_PM();

  return 1; 
}

void i2s_mic_init()
{
    nrfx_i2s_config_t config = NRFX_I2S_DEFAULT_CONFIG(31, 30, 255, 28, 29);
    // In Master mode the MCK frequency and the MCK/LRCK ratio should be
    // set properly in order to achieve desired audio sample rate (which
    // is equivalent to the LRCK frequency).
    // For the following settings we'll get the LRCK frequency equal to
    // 15873 Hz (the closest one to 16 kHz that is possible to achieve).
    config.mck_setup = NRF_I2S_MCK_32MDIV21;
    config.ratio     = NRF_I2S_RATIO_96X;
    config.channels  = NRF_I2S_CHANNELS_STEREO;

    IRQ_DIRECT_CONNECT(I2S_IRQn, 0,i2s_isr_handler, 0);

    int err_code = nrfx_i2s_init(&config, data_handler);

    LOG_INF("i2s_mic_init done...\n");
}

void i2s_mic_test()
{

}
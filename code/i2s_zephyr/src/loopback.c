#include <zephyr.h>
#include <nrfx_i2s.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(i2s_zephyr_loopback, LOG_LEVEL_INF);


#define I2S_DATA_BLOCK_WORDS    512
static uint32_t m_buffer_rx[2][I2S_DATA_BLOCK_WORDS];
static uint32_t m_buffer_tx[2][I2S_DATA_BLOCK_WORDS];

// Delay time between consecutive I2S transfers performed in the main loop
// (in milliseconds).
#define PAUSE_TIME          500
// Number of blocks of data to be contained in each transfer.
#define BLOCKS_TO_TRANSFER  20

static uint8_t volatile m_blocks_transferred     = 0;
static uint8_t          m_zero_samples_to_ignore = 0;
static uint16_t         m_sample_value_to_send;
static uint16_t         m_sample_value_expected;
static bool             m_error_encountered;

static uint32_t       * volatile mp_block_to_fill  = NULL;
static uint32_t const * volatile mp_block_to_check = NULL;


static void prepare_tx_data(uint32_t * p_block)
{
    // These variables will be both zero only at the very beginning of each
    // transfer, so we use them as the indication that the re-initialization
    // should be performed.
    if (m_blocks_transferred == 0 && m_zero_samples_to_ignore == 0)
    {
        // Number of initial samples (actually pairs of L/R samples) with zero
        // values that should be ignored - see the comment in 'check_samples'.
        m_zero_samples_to_ignore = 2;
        m_sample_value_to_send   = 0xCAFE;
        m_sample_value_expected  = 0xCAFE;
        m_error_encountered      = false;
    }

    // [each data word contains two 16-bit samples]
    uint16_t i;
    for (i = 0; i < I2S_DATA_BLOCK_WORDS; ++i)
    {
        uint16_t sample_l = m_sample_value_to_send - 1;
        uint16_t sample_r = m_sample_value_to_send + 1;
        ++m_sample_value_to_send;

        uint32_t * p_word = &p_block[i];
        ((uint16_t *)p_word)[0] = sample_l;
        ((uint16_t *)p_word)[1] = sample_r;
    }
}


static bool check_samples(uint32_t const * p_block)
{
    // [each data word contains two 16-bit samples]
    uint16_t i;
    for (i = 0; i < I2S_DATA_BLOCK_WORDS; ++i)
    {
        uint32_t const * p_word = &p_block[i];
        uint16_t actual_sample_l = ((uint16_t const *)p_word)[0];
        uint16_t actual_sample_r = ((uint16_t const *)p_word)[1];

        // Normally a couple of initial samples sent by the I2S peripheral
        // will have zero values, because it starts to output the clock
        // before the actual data is fetched by EasyDMA. As we are dealing
        // with streaming the initial zero samples can be simply ignored.
        if (m_zero_samples_to_ignore > 0 &&
            actual_sample_l == 0 &&
            actual_sample_r == 0)
        {
            --m_zero_samples_to_ignore;
        }
        else
        {
            m_zero_samples_to_ignore = 0;

            uint16_t expected_sample_l = m_sample_value_expected - 1;
            uint16_t expected_sample_r = m_sample_value_expected + 1;
            ++m_sample_value_expected;

            if (actual_sample_l != expected_sample_l ||
                actual_sample_r != expected_sample_r)
            {
                LOG_INF("%3u: %04x/%04x, expected: %04x/%04x (i: %u)",
                    m_blocks_transferred, actual_sample_l, actual_sample_r,
                    expected_sample_l, expected_sample_r, i);
                return false;
            }
        }
    }
    LOG_INF("%3u: OK", m_blocks_transferred);
    return true;
}


static void check_rx_data(uint32_t const * p_block)
{
    ++m_blocks_transferred;

    if (!m_error_encountered)
    {
        m_error_encountered = !check_samples(p_block);
    }

    if (m_error_encountered)
    {
        //bsp_board_led_off(LED_OK);
        //bsp_board_led_invert(LED_ERROR);
    }
    else
    {
        //bsp_board_led_off(LED_ERROR);
        //bsp_board_led_invert(LED_OK);
    }
}


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
            .p_rx_buffer = m_buffer_rx[1],
            .p_tx_buffer = m_buffer_tx[1],
        };
        nrfx_i2s_next_buffers_set(&next_buffers);

        mp_block_to_fill = m_buffer_tx[1];
    }
    else
    {
        mp_block_to_check = p_released->p_rx_buffer;
        // The driver has just finished accessing the buffers pointed by
        // 'p_released'. They can be used for the next part of the transfer
        // that will be scheduled now.
        nrfx_i2s_next_buffers_set(p_released);

        // The pointer needs to be typecasted here, so that it is possible to
        // modify the content it is pointing to (it is marked in the structure
        // as pointing to constant data because the driver is not supposed to
        // modify the provided data).
        mp_block_to_fill = (uint32_t *)p_released->p_tx_buffer;
    }
}

ISR_DIRECT_DECLARE(i2s_isr_handler) 
{
  nrfx_i2s_irq_handler();
  ISR_DIRECT_PM();

  return 1; 
}

void loopback_init()
{
    nrfx_i2s_config_t config = NRFX_I2S_DEFAULT_CONFIG(31, 30, 255, 27, 26);
    // In Master mode the MCK frequency and the MCK/LRCK ratio should be
    // set properly in order to achieve desired audio sample rate (which
    // is equivalent to the LRCK frequency).
    // For the following settings we'll get the LRCK frequency equal to
    // 15873 Hz (the closest one to 16 kHz that is possible to achieve).
    config.sdin_pin  = 26;
    config.sdout_pin = 27;
    config.mck_setup = NRF_I2S_MCK_32MDIV21;
    config.ratio     = NRF_I2S_RATIO_96X;
    config.channels  = NRF_I2S_CHANNELS_STEREO;

    IRQ_DIRECT_CONNECT(I2S0_IRQn, 0,i2s_isr_handler, 0);

    int err_code = nrfx_i2s_init(&config, data_handler);

    LOG_INF("loopback_int done...\n");
}

void loopback_transfer()
{
    LOG_INF("staring loopback_transfer...\n");

    m_blocks_transferred = 0;
    mp_block_to_fill  = NULL;
    mp_block_to_check = NULL;

    prepare_tx_data(m_buffer_tx[0]);
    LOG_INF("after prepare_tx_data...\n");

    nrfx_i2s_buffers_t const initial_buffers = {
        .p_tx_buffer = m_buffer_tx[0],
        .p_rx_buffer = m_buffer_rx[0],
    };
    int err_code = nrfx_i2s_start(&initial_buffers, I2S_DATA_BLOCK_WORDS, 0);
    LOG_INF("err_code = %d...\n", err_code);

    //APP_ERROR_CHECK(err_code);

    LOG_INF("after nrfx_i2s_start...\n");

    do {
        // Wait for an event.
        //__WFE();
        // Clear the event register.
        //__SEV();
        //__WFE();

        if (mp_block_to_fill)
        {
            prepare_tx_data(mp_block_to_fill);
            mp_block_to_fill = NULL;
        }
        if (mp_block_to_check)
        {
            check_rx_data(mp_block_to_check);
            mp_block_to_check = NULL;
        }
    } while (m_blocks_transferred < BLOCKS_TO_TRANSFER);

    nrfx_i2s_stop();
}
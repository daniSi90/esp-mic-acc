/**
 ********************************************************************************
 * @file    peripheral.c
 * @author  Danijel Sipos
 * @version V1.0.0
 * @date    23.06.2025
 * @brief   Implementation of peripheral
 *
 * @par
 ********************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include "peripheral.h"

static i2s_chan_handle_t rx_handle;

static void peripheral_i2s_init(void);

uint8_t peripheral_init(void)
{
    peripheral_i2s_init();

    return 0; // Return 0 on success
}

i2s_chan_handle_t *peripheral_get_i2s_rx_handle(void)
{
    return &rx_handle;
}

static void peripheral_i2s_init(void)
{
#if CONFIG_MIC_SPH0645LM4H
#elif CONFIG_MIC_ATSAMD21
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));

    i2s_pdm_rx_config_t pdm_rx_cfg = {
        .clk_cfg = I2S_PDM_RX_CLK_DEFAULT_CONFIG(CONFIG_I2S_SAMPLE_RATE_HZ),
        /* The default mono slot is the left slot (whose 'select pin' of the PDM microphone is pulled down) */
        .slot_cfg = I2S_PDM_RX_SLOT_DEFAULT_CONFIG(CONFIG_I2S_BIT_SAMPLE, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .clk = GPIO_I2S_CLK,
            .din = GPIO_I2S_DIN,
            .invert_flags = {
                .clk_inv = false,
            },
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_pdm_rx_mode(rx_handle, &pdm_rx_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(rx_handle));
#endif
}

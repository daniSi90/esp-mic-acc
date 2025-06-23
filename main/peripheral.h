/**
 ********************************************************************************
 * @file    peripheral.h
 * @author  Danijel Sipos
 * @date    23.06.2025
 * @brief   Implementation of peripheral
 *
 * @par
 ********************************************************************************
 */

#ifndef _PERIPHERAL_H_
#define _PERIPHERAL_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "sdkconfig.h"
#include "config_gpios.h"
#if CONFIG_MIC_SPH0645LM4H
#include "driver/i2s_std.h"
#elif CONFIG_MIC_ATSAMD21
#include "driver/i2s_pdm.h"
#endif

#define CONFIG_I2S_SAMPLE_RATE_HZ 44100
#define CONFIG_I2S_BIT_SAMPLE 16
#define NUM_CHANNELS (1) // For mono recording only!

#define SAMPLE_SIZE (CONFIG_I2S_BIT_SAMPLE * 1024)
#define BYTE_RATE (CONFIG_I2S_SAMPLE_RATE_HZ * (CONFIG_I2S_BIT_SAMPLE / 8)) * NUM_CHANNELS

#ifdef __cplusplus
extern "C"
{
#endif

    uint8_t peripheral_init(void);
    i2s_chan_handle_t *peripheral_get_i2s_rx_handle(void);

#ifdef __cplusplus
}
#endif

#endif /* _PERIPHERAL_H_ */
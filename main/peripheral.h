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
#include "config_gpios.h"

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

#ifdef __cplusplus
}
#endif

#endif /* _PERIPHERAL_H_ */
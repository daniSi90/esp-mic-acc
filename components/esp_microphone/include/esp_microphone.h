/**
 ********************************************************************************
 * @file    esp_microphone.h
 * @author  Danijel Sipos
 * @date    21.06.2025
 * @brief   Implementation of esp_microphone
 *
 * @par
 ********************************************************************************
 */

#ifndef _ESP_MICROPHONE_H_
#define _ESP_MICROPHONE_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "sdkconfig.h"

#if CONFIG_MIC_SPH0645LM4H
#include "driver/i2s_std.h"
#elif CONFIG_MIC_ATSAMD21
#include "driver/i2s_pdm.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    void esp_mic_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_MICROPHONE_H_ */
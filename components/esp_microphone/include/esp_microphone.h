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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#if CONFIG_MIC_SPH0645LM4H
#include "driver/i2s_std.h"
#elif CONFIG_MIC_ATSAMD21
#include "driver/i2s_pdm.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    i2s_chan_handle_t *p_i2s_handle;        //< Pointer to the I2S channel handle
    size_t             sample_size;         //< Size of the sample buffer
    int16_t           *sample_buffer;       //< Pointer to the sample buffer for storing microphone data
    uint8_t            sample_buffer_index; //< Index for the sample buffer
    size_t             bytes_read;
    TaskHandle_t       task_handle;
} esp_mic_handle_t;

static inline void
esp_mic_set_sample_size(esp_mic_handle_t *p_mic_handle, size_t sample_size)
{
    p_mic_handle->sample_size = sample_size;
}

static inline void
esp_mic_set_i2s_handle(esp_mic_handle_t *p_mic_handle, i2s_chan_handle_t *p_i2s_handle)
{
    p_mic_handle->p_i2s_handle = p_i2s_handle;
}

esp_mic_handle_t *esp_mic_create(void);
int8_t            esp_mic_start(esp_mic_handle_t *p_mic_handle);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_MICROPHONE_H_ */
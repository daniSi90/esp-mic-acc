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
#include "freertos/queue.h"

#if CONFIG_MIC_SPH0645LM4H
#include "driver/i2s_std.h"
#elif CONFIG_MIC_ATSAMD21
#include "driver/i2s_pdm.h"
#endif

#define ESP_MIC_TRANSFER_DATA_OVER_STREAM_BUFFER 0 // Use stream buffer for transferring data
#define ESP_MIC_TRANSFER_DATA_OVER_DOUBLE_BUFFER 1 // Use double buffering for transferring data

#if ESP_MIC_TRANSFER_DATA_OVER_STREAM_BUFFER
#define SAMPLE_BUFFER_NUM 1 // Only a single sample buffer is needed when using stream buffer - data is copied directly to the stream buffer
#elif ESP_MIC_TRANSFER_DATA_OVER_DOUBLE_BUFFER
#define SAMPLE_BUFFER_NUM 2 // Number of sample buffers - Must not be changed
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if ESP_MIC_TRANSFER_DATA_OVER_DOUBLE_BUFFER
typedef struct
{
    int16_t *p_buffer; //< Pointer to the buffer for transferring data
    size_t   size;     //< Size of the buffer
    uint8_t  index;    //< Index of the buffer
} esp_mic_rx_data_t;
#endif

typedef struct
{
    i2s_chan_handle_t *p_i2s_handle;                     //< Pointer to the I2S channel handle
    size_t             sample_size;                      //< Size of the sample buffer
    int16_t           *sample_buffer[SAMPLE_BUFFER_NUM]; //< Pointer to the sample buffer for storing microphone data
    uint8_t            sample_buffer_num;                //< Number of sample buffers
    uint8_t            sample_buffer_index;              //< Index for the sample buffer
    size_t             bytes_read;                       //< Number of bytes read from the microphone
    TaskHandle_t       task_handle;                      //< Task handle for the microphone task
#if ESP_MIC_TRANSFER_DATA_OVER_STREAM_BUFFER
    StreamBufferHandle_t stream_buffer; //< Stream buffer for transferring data between the task and the main application
#elif ESP_MIC_TRANSFER_DATA_OVER_DOUBLE_BUFFER
    QueueHandle_t double_buffer_queue; //< Queue for transferring data between the task and the main application
#endif
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
#if ESP_MIC_TRANSFER_DATA_OVER_DOUBLE_BUFFER
void esp_mic_poll_data(esp_mic_handle_t *p_mic_handle, esp_mic_rx_data_t *p_data);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _ESP_MICROPHONE_H_ */
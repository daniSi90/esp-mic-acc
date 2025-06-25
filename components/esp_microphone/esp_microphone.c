/**
 ********************************************************************************
 * @file    esp_microphone.c
 * @author  Danijel Sipos
 * @date    21.06.2025
 * @brief   Implementation of esp_microphone
 *
 * 1. https://www.adafruit.com/product/3421
 * 2. https://www.adafruit.com/product/3492
 * 3. https://www.adafruit.com/product/2716
 * @par
 ********************************************************************************
 */

#include <string.h>
#include <stdio.h>
#include "esp_microphone.h"

#define LOG_LOCAL_LEVEL ESP_LOG_NONE

static const char *TAG = "esp-mic";
#include "esp_log.h"

#define MIC_TASK_STACK_SIZE 4048
#define MIC_READ_TIMEOUT_MS 1000

static const TickType_t MIC_READ_TIMEOUT_TICKS = pdMS_TO_TICKS(MIC_READ_TIMEOUT_MS);

static void esp_mic_task(void *arg);

esp_mic_handle_t *
esp_mic_create(void)
{
    esp_mic_handle_t *p_mic_handle = malloc(sizeof(esp_mic_handle_t));
    if (p_mic_handle == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for microphone handle");
        return NULL;
    }
    memset(p_mic_handle, 0, sizeof(esp_mic_handle_t)); // Initialize the structure to zero

    return p_mic_handle;
}

int8_t
esp_mic_start(esp_mic_handle_t *p_mic_handle)
{
    // Initialize the microphone hardware
    ESP_LOGI(TAG, "Initializing microphone");
    if (p_mic_handle == NULL)
    {
        ESP_LOGE(TAG, "Microphone handle is NULL");
        return -1;
    }
    if (p_mic_handle->p_i2s_handle == NULL)
    {
        ESP_LOGE(TAG, "I2S handle is NULL");
        return -1;
    }
    if (p_mic_handle->sample_size == 0)
    {
        ESP_LOGE(TAG, "Sample size is zero");
        return -1;
    }

    const size_t SAMPLE_BUFFER_SIZE = p_mic_handle->sample_size * sizeof(int16_t);
    for (uint8_t i = 0; i < SAMPLE_BUFFER_NUM; i++)
    {
        p_mic_handle->sample_buffer[i] = malloc(SAMPLE_BUFFER_SIZE);
        if (p_mic_handle->sample_buffer[i] == NULL)
        {
            ESP_LOGE(TAG, "Failed to allocate sample buffer");
            return -1;
        }
    }

    /// Optional data transfer method stream buffer/double buffer
#if ESP_MIC_TRANSFER_DATA_OVER_STREAM_BUFFER
    p_mic_handle->stream_buffer = xStreamBufferCreate(SAMPLE_BUFFER_SIZE, SAMPLE_BUFFER_SIZE);
    if (p_mic_handle->stream_buffer == NULL)
    {
        ESP_LOGE(TAG, "Failed to create stream buffer");
        return -1;
    }
#elif ESP_MIC_TRANSFER_DATA_OVER_DOUBLE_BUFFER
    p_mic_handle->double_buffer_queue = xQueueCreate(1, sizeof(esp_mic_rx_data_t));
    if (p_mic_handle->double_buffer_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create double buffer queue");
        return -1;
    }
#endif

    xTaskCreate(esp_mic_task, "mic", MIC_TASK_STACK_SIZE, p_mic_handle, tskIDLE_PRIORITY + 2, &p_mic_handle->task_handle);
    if (p_mic_handle->task_handle == NULL)
    {
        ESP_LOGE(TAG, "Failed to create microphone task");
        return -1;
    }

    return 0; // Return 0 on success
}

#if ESP_MIC_TRANSFER_DATA_OVER_DOUBLE_BUFFER
void
esp_mic_poll_data(esp_mic_handle_t *p_mic_handle, esp_mic_rx_data_t *p_data)
{
    if (xQueueReceive(p_mic_handle->double_buffer_queue, p_data, portMAX_DELAY) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to receive data from double buffer queue");
        return;
    }
}
#endif

static void
esp_mic_task(void *arg)
{
    esp_mic_handle_t *p_mic_handle = (esp_mic_handle_t *)arg;
    esp_mic_rx_data_t esp_mic_data;

    while (1)
    {
        if (i2s_channel_read(*p_mic_handle->p_i2s_handle, p_mic_handle->sample_buffer[p_mic_handle->sample_buffer_index], p_mic_handle->sample_size, &p_mic_handle->bytes_read, MIC_READ_TIMEOUT_TICKS) == ESP_OK)
        {
#if ESP_MIC_TRANSFER_DATA_OVER_STREAM_BUFFER
            // clang-format off
            xStreamBufferSend(p_mic_handle->stream_buffer, \
                              p_mic_handle->sample_buffer[p_mic_handle->sample_buffer_index], \
                              p_mic_handle->bytes_read, \
                              portMAX_DELAY);
            // clang-format on
#elif ESP_MIC_TRANSFER_DATA_OVER_DOUBLE_BUFFER
            esp_mic_data.p_buffer = p_mic_handle->sample_buffer[p_mic_handle->sample_buffer_index];
            esp_mic_data.size     = p_mic_handle->bytes_read;
            esp_mic_data.index    = p_mic_handle->sample_buffer_index;
            xQueueOverwrite(p_mic_handle->double_buffer_queue, &esp_mic_data);

            p_mic_handle->sample_buffer_index = !p_mic_handle->sample_buffer_index; // Toggle the index for double buffering
#endif
        }
    }
}
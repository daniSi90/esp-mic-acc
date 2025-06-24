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

#include "esp_microphone.h"
#include <stdio.h>

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

    p_mic_handle->p_i2s_handle        = NULL; // Initialize to NULL
    p_mic_handle->sample_size         = 0;    // Initialize to zero
    p_mic_handle->sample_buffer       = NULL; // Initialize to NULL
    p_mic_handle->sample_buffer_index = 1;    // Initialize to zero
    p_mic_handle->task_handle         = NULL; // Initialize to NULL

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

    p_mic_handle->sample_buffer = malloc(p_mic_handle->sample_size * sizeof(int16_t));
    if (p_mic_handle->sample_buffer == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate sample buffer");
        return -1;
    }

    xTaskCreate(esp_mic_task, "mic", MIC_TASK_STACK_SIZE, p_mic_handle, tskIDLE_PRIORITY + 2, &p_mic_handle->task_handle);
    if (p_mic_handle->task_handle == NULL)
    {
        ESP_LOGE(TAG, "Failed to create microphone task");
        return -1;
    }

    return 0; // Return 0 on success
}

static void
esp_mic_task(void *arg)
{
    esp_mic_handle_t *p_mic_handle = (esp_mic_handle_t *)arg;
    ESP_LOGI(TAG, "I2S handle: %p", p_mic_handle->p_i2s_handle);
    vTaskDelay(pdMS_TO_TICKS(1000)); // Wait for I2S to be ready
    while (1)
    {
        if (i2s_channel_read(*p_mic_handle->p_i2s_handle, p_mic_handle->sample_buffer, p_mic_handle->sample_size, &p_mic_handle->bytes_read, MIC_READ_TIMEOUT_TICKS) == ESP_OK)
        {
            ESP_LOGI(TAG, "Read %zu bytes from microphone", p_mic_handle->bytes_read);

#if 0
            for (size_t i = 0; i < 100; i++)
            {
                printf("%d, ", p_mic_handle->sample_buffer[i]);
            }
            printf("\n\n\n");
#endif
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Simulate some work
    }
}
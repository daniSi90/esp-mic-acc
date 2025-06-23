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

static const char *TAG = "esp-mic";
#include "esp_log.h"

#define MIC_TASK_STACK_SIZE 2048
#define MIC_READ_TIMEOUT_MS 1000

static const TickType_t MIC_READ_TIMEOUT_TICKS = pdMS_TO_TICKS(1000);

int8_t esp_mic_start(esp_mic_handle_t *p_mic_handle)
{
  // Initialize the microphone hardware
  ESP_LOGI(TAG, "Initializing microphone");
  if (p_mic_handle == NULL)
  {
    ESP_LOGE(TAG, "Microphone handle is NULL");
    return -1;
  }
  xTaskCreate(NULL, "NAME", MIC_TASK_STACK_SIZE, p_mic_handle, tskIDLE_PRIORITY + 2, &p_mic_handle->task_handle);

  if (p_mic_handle->task_handle == NULL)
  {
    ESP_LOGE(TAG, "Failed to create microphone task");
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

  return 0; // Return 0 on success
}

void esp_mic_task(void *arg)
{
  esp_mic_handle_t *p_mic_handle = (esp_mic_handle_t *)arg;
  i2s_chan_handle_t i2s_handle = *p_mic_handle->p_i2s_handle;

  while (1)
  {
    vTaskDelay(pdMS_TO_TICKS(1000)); // Simulate some work

    if (i2s_channel_read(i2s_handle, p_mic_handle->sample_buffer, p_mic_handle->sample_size, &p_mic_handle->bytes_read, MIC_READ_TIMEOUT_TICKS) == ESP_OK)
    {
      /// TODO: Process the read data
    }
  }
}
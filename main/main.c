#include "esp_chip_info.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <inttypes.h>
#include <stdio.h>
#include "peripheral.h"
#include "esp_microphone.h"
#include "udp_handler.h"
#include <math.h>

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"

static const char *TAG = "main";
#if 0
void
app_main(void)
{
    ESP_LOGI(TAG, "Init peripheral");
    peripheral_init();

    udp_handler_init();

    i2s_chan_handle_t *p_i2s_rx_handle  = peripheral_get_i2s_rx_handle();
    esp_mic_handle_t  *p_esp_mic_handle = esp_mic_create();
    if (p_esp_mic_handle != NULL)
    {
        esp_mic_set_i2s_handle(p_esp_mic_handle, p_i2s_rx_handle);
        esp_mic_set_sample_size(p_esp_mic_handle, SAMPLE_SIZE); //
        esp_mic_start(p_esp_mic_handle);
    }

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#else
#define SAMPLE_SIZE SAMPLES_PER_PACKET // Match the UDP packet size

void
app_main(void)
{
    ESP_LOGI(TAG, "Initializing system");

    // Initialize peripherals
    peripheral_init();
    udp_handler_init();

    // Setup microphone
    i2s_chan_handle_t *p_i2s_rx_handle  = peripheral_get_i2s_rx_handle();
    esp_mic_handle_t  *p_esp_mic_handle = esp_mic_create();

    if (p_esp_mic_handle != NULL)
    {
        esp_mic_set_i2s_handle(p_esp_mic_handle, p_i2s_rx_handle);
        esp_mic_set_sample_size(p_esp_mic_handle, SAMPLE_SIZE);
        esp_mic_start(p_esp_mic_handle);
    }

    // Main processing loop
    esp_mic_rx_data_t mic_data;
    while (1)
    {
        esp_mic_poll_data(p_esp_mic_handle, &mic_data);

        // Send audio over UDP
        udp_send_audio(mic_data.p_buffer, mic_data.size / sizeof(int16_t));

        // Small delay to prevent overwhelming the system
        // vTaskDelay(pdMS_TO_TICKS(10));
    }
}
#endif
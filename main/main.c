#include "esp_chip_info.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <inttypes.h>
#include <stdio.h>
#include "peripheral.h"
#include "esp_microphone.h"
#include "esp_analog_microphone.h"
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
// #define SAMPLE_SIZE SAMPLES_PER_PACKET // Match the UDP packet size

void
app_main(void)
{
    ESP_LOGI(TAG, "Initializing system");

    // Initialize peripherals
    peripheral_init();
    udp_handler_init();

    esp_analog_mic_init();
#if 0
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
#else
    esp_anal_mic_data_t mic_data;
    while (1)
    {
        esp_analog_mic_read(&mic_data);
        ESP_LOGI(TAG,
                 "Read %zu samples %u %u %u %u %u %u %u %u %u %u %u %u",
                 mic_data.sample_count,
                 (unsigned int)mic_data.p_data[0],
                 (unsigned int)mic_data.p_data[1],
                 (unsigned int)mic_data.p_data[2],
                 (unsigned int)mic_data.p_data[3],
                 (unsigned int)mic_data.p_data[4],
                 (unsigned int)mic_data.p_data[5],
                 (unsigned int)mic_data.p_data[6],
                 (unsigned int)mic_data.p_data[7],
                 (unsigned int)mic_data.p_data[8],
                 (unsigned int)mic_data.p_data[9],
                 (unsigned int)mic_data.p_data[10],
                 (unsigned int)mic_data.p_data[11]);
    }

#endif
}
#endif
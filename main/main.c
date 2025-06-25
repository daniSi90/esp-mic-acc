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

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"


static const char *TAG = "main";

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

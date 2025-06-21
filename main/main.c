#include "esp_chip_info.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <inttypes.h>
#include <stdio.h>
#include "esp_microphone.h"

#define LOG_LOCAL_LEVEL ESP_LOG_NONE

#include "esp_log.h"

static const char *TAG = "main";

void app_main(void)
{
  ESP_LOGI(TAG, "Starting application...");

  while (1)
  {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

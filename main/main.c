#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <inttypes.h>
#include <stdio.h>

#include "esp_log.h"

#include "udp_handler.h"

static const char *TAG = "main";

void app_main(void) {
  ESP_LOGI(TAG, "Starting application...");
  udp_handler_init();

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

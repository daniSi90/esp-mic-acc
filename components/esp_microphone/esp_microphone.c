/**
 ********************************************************************************
 * @file    esp_microphone.c
 * @author  Danijel Sipos
 * @date    21.06.2025
 * @brief   Implementation of esp_microphone
 *
 * @par
 ********************************************************************************
 */

#include "esp_microphone.h"
#include <stdio.h>

static const char *TAG = "esp-mic";
#include "esp_log.h"

/**
 * 1. https://www.adafruit.com/product/3421
 * 2. https://www.adafruit.com/product/3492
 * 3. https://www.adafruit.com/product/2716
 */

void ewsp_mic_init(void) {
  // Initialize the microphone hardware
  printf("Microphone initialized.\n");
}

/**
 ********************************************************************************
 * @file    esp_analog_microphone.c
 * @author  Danijel Sipos
 * @date    19.07.2025
 * @brief   Implementation of esp_analog_microphone
 *
 * NOTE: 1. When CONFIG_ADC_CONTINUOUS_ISR_IRAM_SAFE is enabled, the callback itself and functions called
 *          by it should be placed in IRAM. Involved variables (including user_data) should be in internal
 *          RAM as well.
 ********************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "esp_analog_microphone.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "soc/gpio_num.h"
#include "esp_adc/adc_continuous.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO

static const char *TAG = "esp-analog-mic";
#include "esp_log.h"

// #define ANALOG_MIC_SAMPLE_RATE 44100 // Sample rate (Hz)
#define ANALOG_MIC_SAMPLE_RATE 4400 // Sample rate (Hz)
#define ADC_MAX_BUFFER_SIZE    1024 // Max buffer size for ADC continuous mode
#define ADC_FRAME_SIZE         256  // Size of each ADC frame in bytes

static adc_continuous_config_t adc_contin_config;
static adc_continuous_handle_t handle    = NULL;
static QueueHandle_t           adc_queue = NULL;

static bool esp_analog_mic_adc_continuous_conv_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data);
static bool esp_analog_mic_adc_continuous_pool_ovf_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data);

void
esp_analog_mic_init(void)
{
    ESP_LOGI(TAG, "Initializing analog microphone");

    /// 2. Initialization needed
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = ADC_MAX_BUFFER_SIZE,
        .conv_frame_size    = ADC_FRAME_SIZE,
        .flags.flush_pool   = true, // Flush the internal pool when the pool is full
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));

    /// 1. Get ADC unit and channel from GPIO
    adc_unit_t    adc_unit;
    adc_channel_t adc_channel;
    ESP_ERROR_CHECK(adc_continuous_io_to_channel(GPIO_NUM_4, &adc_unit, &adc_channel));

    adc_digi_pattern_config_t adc_pattern = {
        .atten     = ADC_ATTEN_DB_12, // Set attenuation as needed
        .channel   = adc_channel,
        .unit      = adc_unit,
        .bit_width = ADC_BITWIDTH_12, // Set bitwidth as needed
    };

    adc_contin_config.pattern_num    = 1;
    adc_contin_config.adc_pattern    = &adc_pattern;
    adc_contin_config.sample_freq_hz = ANALOG_MIC_SAMPLE_RATE;                                                     // Set sample frequency as needed
    adc_contin_config.conv_mode      = (adc_unit == ADC_UNIT_1) ? ADC_CONV_SINGLE_UNIT_1 : ADC_CONV_SINGLE_UNIT_2; // Use single unit conversion mode
    adc_contin_config.format         = ADC_DIGI_OUTPUT_FORMAT_TYPE2;                                               // Use output format type 1

    adc_continuous_config(handle, &adc_contin_config);

    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = esp_analog_mic_adc_continuous_conv_done_cb, // Set your callback function here if needed
        .on_pool_ovf  = esp_analog_mic_adc_continuous_pool_ovf_cb,  // Set your callback function here if needed
    };
    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(handle, &cbs, NULL));

    adc_queue = xQueueCreate(1, sizeof(esp_anal_mic_data_t));

    ESP_ERROR_CHECK(adc_continuous_start(handle)); // Start the ADC continuous mode
}

uint8_t
esp_analog_mic_read(esp_anal_mic_data_t *mic_read)
{
    if (xQueueReceive(adc_queue, mic_read, portMAX_DELAY) != pdTRUE)
    {
        /// Failed to receive data from ADC queue
        return -1;
    }
    return 0; // Return 0 on success
}

static bool
esp_analog_mic_adc_continuous_conv_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    BaseType_t          higher_prio_task_woken = pdFALSE;
    esp_anal_mic_data_t mic_data;

    mic_data.p_data       = edata->conv_frame_buffer;
    mic_data.sample_count = edata->size;

    /// NOTE: Items are queued by copy not reference so it is preferable to only queue small items,
    ///  especially when called from an ISR. In most cases it would be preferable to store a pointer to the item being queued.
    xQueueSendFromISR(adc_queue, &mic_data, &higher_prio_task_woken);

    if (higher_prio_task_woken)
    {
        portYIELD_FROM_ISR();
    }

    return true;
}

static bool
esp_analog_mic_adc_continuous_pool_ovf_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    return true;
}
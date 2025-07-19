/**
 ********************************************************************************
 * @file    esp_analog_microphone.h
 * @author  Danijel Sipos
 * @date    19.07.2025
 * @brief   Implementation of esp_analog_microphone
 *
 ********************************************************************************
 */

#ifndef _ESP_ANALOG_MICROPHONE_H_
#define _ESP_ANALOG_MICROPHONE_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t *p_data;       // Pointer to the data buffer
    size_t   sample_count; // Number of samples in the data
} esp_anal_mic_data_t;

void    esp_analog_mic_init(void);
uint8_t esp_analog_mic_read(esp_anal_mic_data_t *mic_read);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_ANALOG_MICROPHONE_H_ */
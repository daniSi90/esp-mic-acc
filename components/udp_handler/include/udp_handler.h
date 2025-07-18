/**
 ********************************************************************************
 * @file    udp_handler.h
 * @author  Stefan M.
 * @date    2025-06-21
 * @brief
 *
 ********************************************************************************
 */

#ifndef _UDP_HANDLER_H_
#define _UDP_HANDLER_H_

#include <stdint.h>
#include "esp_microphone.h"

#ifdef __cplusplus
extern "C" {
#endif

// Audio packet configuration
#define AUDIO_CHANNELS        1   // Mono
#define AUDIO_BITS_PER_SAMPLE 16  // 16-bit PCM
#define SAMPLES_PER_PACKET    512 // ~32ms of audio at 16kHz
#define UDP_PACKET_SIZE       (SAMPLES_PER_PACKET * sizeof(int16_t))

typedef struct
{
    uint32_t sequence_num;
    uint32_t timestamp;
    int16_t  audio_data[SAMPLES_PER_PACKET];
} audio_packet_t;

void udp_handler_init(void);
void udp_send_audio(const int16_t *audio_data, size_t samples_count);

#ifdef __cplusplus
}
#endif

#endif /* _UDP_HANDLER_H_ */
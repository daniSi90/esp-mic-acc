#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "nvs_flash.h"
#include <lwip/netdb.h>
#include <math.h>
#include <string.h>

#define TAG "UDP_WAVE_GENERATOR"
#define PORT 5678
#define ESP_IP "192.168.1.100"     // New ESP32 IP (same subnet as PC)
#define GATEWAY_IP "192.168.1.1"   // Your router's LAN IP
#define HOST_IP_ADDR "192.168.4.2" // Your PC's Ethernet IP (enp6s0)
#define NETMASK "255.255.255.0"
#define SAMPLE_COUNT 1000
#define SAMPLE_FREQUENCY 1000.0f // 1 kHz sample frequency
#define SIGNAL_FREQUENCY 50.0f   // 50 Hz sine wave

/* FreeRTOS event group to signal when we're connected */
static EventGroupHandle_t wifi_event_group;
/* The event group allows multiple bits for each event */
#define WIFI_CONNECTED_BIT BIT0

static void udp_client_task(void *pvParameters);

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
    wifi_event_ap_staconnected_t *event =
        (wifi_event_ap_staconnected_t *)event_data;
    ESP_LOGI(TAG, "Station connected: MAC=%02x:%02x:%02x:%02x:%02x:%02x",
             event->mac[0], event->mac[1], event->mac[2], event->mac[3],
             event->mac[4], event->mac[5]);
  }
}

void wifi_init_ap() {
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // AP configuration
  wifi_config_t wifi_config = {
      .ap =
          {
              .ssid = "ESP32_UDP_AP",     // Name of the AP
              .password = "testpassword", // Password (min 8 chars)
              .max_connection = 1,        // Only allow 1 client (your PC)
              .authmode = WIFI_AUTH_WPA2_PSK,
          },
  };

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  // Set static IP for AP (optional but recommended)
  esp_netif_ip_info_t ip_info;
  IP4_ADDR(&ip_info.ip, 192, 168, 4, 1);        // AP IP
  IP4_ADDR(&ip_info.gw, 192, 168, 4, 1);        // Gateway
  IP4_ADDR(&ip_info.netmask, 255, 255, 255, 0); // Subnet mask
  esp_netif_dhcps_stop(ap_netif);
  esp_netif_set_ip_info(ap_netif, &ip_info);
  esp_netif_dhcps_start(ap_netif);

  ESP_LOGI(TAG, "AP started. SSID: %s", wifi_config.ap.ssid);
}

typedef struct {
  float sample_rate;
  uint32_t sample_count;
  float samples[SAMPLE_COUNT];
} wave_packet_t;

static void generate_sine_wave(wave_packet_t *packet) {
  packet->sample_rate = SAMPLE_FREQUENCY;
  packet->sample_count = SAMPLE_COUNT;

  for (int i = 0; i < SAMPLE_COUNT; i++) {
    float t = (float)i / SAMPLE_FREQUENCY;
    packet->samples[i] = sinf(2 * M_PI * SIGNAL_FREQUENCY * t);
  }
}

static void udp_client_task(void *pvParameters) {
  char rx_buffer[128];
  struct sockaddr_in dest_addr = {.sin_addr.s_addr = inet_addr(HOST_IP_ADDR),
                                  .sin_family = AF_INET,
                                  .sin_port = htons(PORT)};

  // Generate the sine wave packet once (could be regenerated periodically if
  // needed)
  wave_packet_t wave_packet;
  generate_sine_wave(&wave_packet);

  while (1) {
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
      ESP_LOGE(TAG, "Socket creation failed: %s", strerror(errno));
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      continue;
    }

    struct timeval timeout = {.tv_sec = 2, .tv_usec = 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    ESP_LOGI(TAG, "Sending sine wave to %s:%d", inet_ntoa(dest_addr.sin_addr),
             ntohs(dest_addr.sin_port));

    // Send the entire wave packet structure
    int err = sendto(sock, &wave_packet, sizeof(wave_packet), 0,
                     (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err < 0) {
      ESP_LOGE(TAG, "Send failed: %s", strerror(errno));
      close(sock);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      continue;
    }

    ESP_LOGI(TAG, "Sent %u bytes of wave data (%.1f Hz, %" PRIu32 " samples)",
             (unsigned int)sizeof(wave_packet), wave_packet.sample_rate,
             wave_packet.sample_count);

    // Optional: Wait for acknowledgment
    struct sockaddr_in source_addr;
    socklen_t socklen = sizeof(source_addr);
    int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0,
                       (struct sockaddr *)&source_addr, &socklen);

    if (len > 0) {
      rx_buffer[len] = 0;
      ESP_LOGI(TAG, "Received %d bytes: %s", len, rx_buffer);
    }

    close(sock);
    vTaskDelay(2000 / portTICK_PERIOD_MS); // Send every 2 seconds
  }
}

void udp_handler_init() {
  ESP_ERROR_CHECK(nvs_flash_init());
  wifi_init_ap(); // Use AP mode instead of STA mode
  xTaskCreate(udp_client_task, "udp_client", 4096 * 2, NULL, 5, NULL);
}
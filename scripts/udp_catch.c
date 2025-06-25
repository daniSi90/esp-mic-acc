#define _GNU_SOURCE
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <net/ethernet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT 5678
#define LOG_FILE "udp_packets.log"

void write_log(FILE *log_file, const char *src_ip, uint16_t src_port,
               const char *data, int data_len) {
  time_t now;
  time(&now);
  char timestamp[32];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

  // JSON-like format for easy parsing in Python
  fprintf(log_file, "{\n");
  fprintf(log_file, "  \"timestamp\": \"%s\",\n", timestamp);
  fprintf(log_file, "  \"source_ip\": \"%s\",\n", src_ip);
  fprintf(log_file, "  \"source_port\": %d,\n", src_port);
  fprintf(log_file, "  \"data_length\": %d,\n", data_len);
  fprintf(log_file, "  \"data\": \"");

  // Write data as hex string for reliability (handles non-printable chars)
  for (int i = 0; i < data_len; i++) {
    fprintf(log_file, "%02x", (unsigned char)data[i]);
  }
  fprintf(log_file, "\"\n},\n");
  fflush(log_file); // Ensure data is written immediately
}

int main() {
  // Open log file in append mode
  FILE *log_file = fopen(LOG_FILE, "a");
  if (!log_file) {
    perror("fopen");
    return 1;
  }

  // Create raw socket
  int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (sock < 0) {
    perror("socket");
    fclose(log_file);
    return 1;
  }

  // Increase receive buffer
  int rcvbuf = 26214400;
  if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf))) {
    perror("setsockopt");
  }

  printf("Listening for raw UDP packets on port %d...\n", PORT);
  printf("Logging to %s\n", LOG_FILE);

  char buffer[65536];
  while (1) {
    // Receive packet
    ssize_t len = recv(sock, buffer, sizeof(buffer), 0);
    if (len <= 0) {
      perror("recv");
      continue;
    }

    // Parse Ethernet header
    struct ethhdr *eth = (struct ethhdr *)buffer;

    // Only process IP packets
    if (ntohs(eth->h_proto) != ETH_P_IP) {
      continue;
    }

    // Parse IP header
    struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));

    // Only process UDP packets
    if (ip->protocol != IPPROTO_UDP) {
      continue;
    }

    // Parse UDP header
    struct udphdr *udp =
        (struct udphdr *)(buffer + sizeof(struct ethhdr) + (ip->ihl * 4));

    // Only process packets for our port
    if (ntohs(udp->dest) != PORT) {
      continue;
    }

    // Get payload data
    char *data =
        buffer + sizeof(struct ethhdr) + (ip->ihl * 4) + sizeof(struct udphdr);
    int data_len = len - (data - buffer);

    // Get source IP and port
    char src_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip->saddr, src_ip, sizeof(src_ip));
    uint16_t src_port = ntohs(udp->source);

    // Print to console (optional)
    printf("Received %d bytes from %s:%d\n", data_len, src_ip, src_port);

    // Write to log file
    write_log(log_file, src_ip, src_port, data, data_len);
  }

  close(sock);
  fclose(log_file);
  return 0;
}
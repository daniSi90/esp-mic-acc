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
#include <unistd.h>

#define PORT 5678

int main() {
  // Create raw socket
  int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (sock < 0) {
    perror("socket");
    return 1;
  }

  // Increase receive buffer
  int rcvbuf = 26214400;
  if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf))) {
    perror("setsockopt");
  }

  printf("Listening for raw UDP packets on port %d...\n", PORT);

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

    // Print source and data
    char src_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip->saddr, src_ip, sizeof(src_ip));
    printf("Received %d bytes from %s:%d: %.*s\n", data_len, src_ip,
           ntohs(udp->source), data_len, data);
  }

  close(sock);
  return 0;
}
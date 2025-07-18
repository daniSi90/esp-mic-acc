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
#include <signal.h>

#define PORT     5678
#define LOG_FILE "udp_packets.json"

volatile sig_atomic_t running = 1;

void
handle_signal(int sig)
{
    running = 0;
}

void
write_log(FILE *log_file, const char *src_ip, uint16_t src_port, const char *data, int data_len, int is_first_packet)
{
    time_t now;
    time(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    // Don't write comma for first packet
    if (!is_first_packet)
    {
        fprintf(log_file, ",\n");
    }

    // JSON format
    fprintf(log_file, "  {\n");
    fprintf(log_file, "    \"timestamp\": \"%s\",\n", timestamp);
    fprintf(log_file, "    \"source_ip\": \"%s\",\n", src_ip);
    fprintf(log_file, "    \"source_port\": %d,\n", src_port);
    fprintf(log_file, "    \"data_length\": %d,\n", data_len);
    fprintf(log_file, "    \"data\": \"");

    // Write data as hex string
    for (int i = 0; i < data_len; i++)
    {
        fprintf(log_file, "%02x", (unsigned char)data[i]);
    }
    fprintf(log_file, "\"\n  }");
    fflush(log_file);
}

int
main()
{
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    // Open log file in write mode (overwrite existing)
    FILE *log_file = fopen(LOG_FILE, "w");
    if (!log_file)
    {
        perror("fopen");
        return 1;
    }

    // Write JSON array start
    fprintf(log_file, "[\n");
    fflush(log_file);

    // Create raw socket
    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock < 0)
    {
        perror("socket");
        fclose(log_file);
        return 1;
    }

    // Increase receive buffer
    int rcvbuf = 26214400;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf)))
    {
        perror("setsockopt");
    }

    printf("Listening for raw UDP packets on port %d...\n", PORT);
    printf("Logging to %s\n", LOG_FILE);

    char buffer[65536];
    int  is_first_packet = 1;

    while (running)
    {
        // Receive packet
        ssize_t len = recv(sock, buffer, sizeof(buffer), 0);
        if (len <= 0)
        {
            perror("recv");
            continue;
        }

        // Parse Ethernet header
        struct ethhdr *eth = (struct ethhdr *)buffer;

        // Only process IP packets
        if (ntohs(eth->h_proto) != ETH_P_IP)
        {
            continue;
        }

        // Parse IP header
        struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));

        // Only process UDP packets
        if (ip->protocol != IPPROTO_UDP)
        {
            continue;
        }

        // Parse UDP header
        struct udphdr *udp = (struct udphdr *)(buffer + sizeof(struct ethhdr) + (ip->ihl * 4));

        // Only process packets for our port
        if (ntohs(udp->dest) != PORT)
        {
            int expected_size = 512 * 2 + 8; // SAMPLES_PER_PACKET * 2 + header
            if (ntohs(udp->len) != expected_size)
            {
                continue; // Skip malformed packets
            }
        }

        // Get payload data
        int   payload_offset = sizeof(struct ethhdr) + (ip->ihl * 4) + sizeof(struct udphdr);
        char *data           = buffer + payload_offset;
        int   data_len       = ntohs(udp->len) - sizeof(struct udphdr); // Use UDP length field

        // Get source IP and port
        char src_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ip->saddr, src_ip, sizeof(src_ip));
        uint16_t src_port = ntohs(udp->source);

        // Print to console (optional)
        printf("Received %d bytes from %s:%d\n", data_len, src_ip, src_port);

        // Write to log file
        write_log(log_file, src_ip, src_port, data, data_len, is_first_packet);
        is_first_packet = 0;
    }

    fprintf(log_file, "\n]"); // Close JSON array
    fclose(log_file);
    close(sock);
    return 0;
}
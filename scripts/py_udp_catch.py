#!/usr/bin/python3
# minimal_udp_receiver.py
import socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("0.0.0.0", 5678))
print("Listening...")
while True:
    data, addr = sock.recvfrom(1024)
    print(f"Received: {data.decode()} from {addr}")
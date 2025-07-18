#!/usr/bin/python3
import json
import struct
import matplotlib.pyplot as plt

# Load packets from udp_packets.json
with open("udp_packets.json", "r") as f:
    packets = json.load(f)

all_samples = []

for packet in packets:
    hex_data = packet["data"]
    raw = bytes.fromhex(hex_data)

    # Make sure packet is at least 8 bytes (seq_num + timestamp)
    if len(raw) < 8:
        continue

    # Unpack sequence number and timestamp (but we don't need them here)
    sequence_num, timestamp = struct.unpack("<II", raw[:8])

    # The rest is audio data (int16_t samples)
    audio_data = raw[8:]
    samples = list(struct.iter_unpack("<h", audio_data))

    # Flatten tuples (from iter_unpack) and append
    all_samples.extend([s[0] for s in samples])

# Plot the audio samples
plt.figure(figsize=(12, 5))
plt.plot(all_samples, label="Audio Samples")
plt.title("UDP Audio Samples")
plt.xlabel("Sample Index")
plt.ylabel("Amplitude (int16)")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()

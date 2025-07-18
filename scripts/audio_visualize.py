#!/usr/bin/python3
import json
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime
import struct

# Configuration
SAMPLE_RATE = 44100  # 16kHz sample rate (adjust if your data is different)
CHUNK_SIZE = 512     # Samples per packet (should match ESP32 setting)

def process_packets(json_data):
    """Process JSON packets and extract audio data"""
    all_samples = []
    timestamps = []
    
    for packet in json_data:
        # Convert hex data to bytes
        hex_data = packet['data']
        byte_data = bytes.fromhex(hex_data)
        
        # The first 8 bytes are metadata (sequence_num and timestamp)
        # The rest are 16-bit signed integers (little-endian)
        samples = []
        for i in range(8, len(byte_data), 2):
            # Unpack 16-bit signed integer (little-endian)
            sample = struct.unpack('<h', byte_data[i:i+2])[0]
            samples.append(sample)
        
        # Convert to numpy array and normalize
        samples = np.array(samples, dtype=np.float32)
        samples = samples / 32768.0  # Normalize to [-1, 1]
        
        all_samples.extend(samples)
        
        # Parse timestamp
        ts = datetime.strptime(packet['timestamp'], "%Y-%m-%d %H:%M:%S")
        timestamps.append(ts)
    
    return np.array(all_samples), timestamps

def plot_waveform(samples, sample_rate):
    """Plot the waveform with proper time axis"""
    # Create time axis
    duration = len(samples) / sample_rate
    time = np.linspace(0, duration, num=len(samples))
    
    # Plot
    plt.figure(figsize=(12, 4))
    plt.plot(time, samples)
    plt.title('Audio Waveform')
    plt.xlabel('Time (s)')
    plt.ylabel('Amplitude')
    plt.grid(True)
    
    # Show spectrogram too
    plt.figure(figsize=(12, 4))
    plt.specgram(samples, Fs=sample_rate, NFFT=1024, noverlap=512)
    plt.title('Spectrogram')
    plt.xlabel('Time (s)')
    plt.ylabel('Frequency (Hz)')
    plt.colorbar(label='Intensity (dB)')
    
    plt.tight_layout()
    plt.show()

def main():
    # Load your JSON data (replace this with your actual data loading method)
    with open('udp_packets.json') as f:
        json_data = json.load(f)
    
    # Process packets
    samples, timestamps = process_packets(json_data)
    
    print(f"Processed {len(samples)} samples from {len(timestamps)} packets")
    print(f"First timestamp: {timestamps[0]}, Last timestamp: {timestamps[-1]}")
    print(f"Duration: {len(samples)/SAMPLE_RATE:.2f} seconds")
    
    # Plot waveform
    plot_waveform(samples, SAMPLE_RATE)

if __name__ == "__main__":
    main()
#!/usr/bin/python3
#!/usr/bin/python3
import json
import struct
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import EngFormatter
from datetime import datetime
import csv

def parse_log_file(log_file_path):
    with open(log_file_path, 'r') as f:
        content = f.read()
    
    try:
        json_content = "[" + content[:-2] + "]"
        packets = json.loads(json_content)
    except (json.JSONDecodeError, IndexError) as e:
        print(f"Error parsing log file: {e}")
        return None, None, None

    all_samples = []
    packet_boundaries = []
    timestamps = []
    sample_rate = None
    current_sample_index = 0
    
    # Additional structures for CSV output
    packet_info = []  # Will store which packet each sample belongs to
    sample_timestamps = []  # Will store precise timestamps for each sample
    
    for packet_idx, packet in enumerate(packets):
        if packet['data_length'] <= 20:
            continue
            
        hex_data = packet['data']
        byte_data = bytes.fromhex(hex_data)
        
        try:
            # Parse timestamp
            timestamp = datetime.strptime(packet['timestamp'], "%Y-%m-%d %H:%M:%S")
            timestamps.append((current_sample_index, timestamp))
            
            # Parse packet header
            sample_rate = struct.unpack_from('f', byte_data, 0)[0]
            sample_count = struct.unpack_from('I', byte_data, 4)[0]
            
            available_bytes = len(byte_data) - 8
            max_possible_samples = available_bytes // 4
            actual_samples = min(sample_count, max_possible_samples)
            
            if actual_samples < sample_count:
                print(f"Warning: Packet truncated - expected {sample_count} samples, got {actual_samples}")
            
            # Extract samples
            samples = struct.unpack_from(f'{actual_samples}f', byte_data, 8)
            all_samples.extend(samples)
            
            # Calculate sample timestamps for this packet
            packet_duration = actual_samples / sample_rate
            sample_times = np.linspace(0, packet_duration, actual_samples, endpoint=False)
            for t in sample_times:
                sample_timestamps.append(timestamp.timestamp() + t)
            
            # Record packet info for each sample
            packet_info.extend([packet_idx] * actual_samples)
            
            # Record packet boundary
            packet_boundaries.append(current_sample_index)
            current_sample_index += actual_samples
            
        except (struct.error, ValueError) as e:
            print(f"Error processing packet: {e}")
            continue
    
    return np.array(all_samples), sample_rate, {
        'boundaries': packet_boundaries, 
        'timestamps': timestamps,
        'packet_info': packet_info,
        'sample_timestamps': sample_timestamps
    }

def save_to_csv(samples, sample_rate, metadata, filename='waveform_data.csv', minimal=False):
    """Save samples to a CSV file
    
    Args:
        samples: Array of sample values
        sample_rate: Sampling rate in Hz
        metadata: Dictionary containing sample metadata
        filename: Output CSV filename
        minimal: If True, only saves index and sample values. If False (default), saves all metadata.
    """
    if samples is None or sample_rate is None or len(samples) == 0:
        print("No valid wave data found to save to CSV")
        return
    
    print(f"Saving waveform data to {filename}...")
    
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        
        if minimal:
            # Minimal version - just index and sample
            writer.writerow(['Sample Index', 'Amplitude'])
            for i, sample in enumerate(samples):
                writer.writerow([i, sample])
        else:
            # Full version with all metadata
            writer.writerow([
                'Sample Index', 
                'Timestamp', 
                'Human Readable Time', 
                'Packet Index', 
                'Amplitude'
            ])
            for i in range(len(samples)):
                human_time = datetime.fromtimestamp(metadata['sample_timestamps'][i]).strftime("%Y-%m-%d %H:%M:%S.%f")
                writer.writerow([
                    i,
                    metadata['sample_timestamps'][i],
                    human_time,
                    metadata['packet_info'][i],
                    samples[i]
                ])
def plot_waveform(samples, sample_rate, metadata):
    if samples is None or sample_rate is None or len(samples) == 0:
        print("No valid wave data found to plot")
        return
    
    # Create time axis
    duration = len(samples) / sample_rate
    time_axis = np.linspace(0, duration, len(samples))
    
    # Create plot
    plt.figure(figsize=(14, 7))
    plt.plot(time_axis, samples, linewidth=0.5, label='Waveform')
    
    # Mark packet boundaries
    boundaries = metadata['boundaries']
    timestamps = metadata['timestamps']
    
    if boundaries:
        for i, boundary in enumerate(boundaries):
            if boundary < len(time_axis):
                plt.axvline(x=time_axis[boundary], color='r', linestyle='--', alpha=0.3, linewidth=0.7)
                
                # Annotate every 5th packet to avoid clutter
                if i % 5 == 0 and i < len(timestamps):
                    ts = timestamps[i][1].strftime("%H:%M:%S")
                    plt.text(time_axis[boundary], max(samples)*0.9, 
                            f"Packet {i}\n{ts}", 
                            ha='right', va='top', fontsize=8,
                            bbox=dict(facecolor='white', alpha=0.7, edgecolor='none'))
    
    plt.title(f"Waveform with Packet Boundaries\nSample Rate: {sample_rate} Hz, Total Samples: {len(samples)}")
    plt.xlabel("Time (seconds)")
    plt.ylabel("Amplitude")
    
    # Add legend and grid
    plt.legend(loc='upper right')
    plt.grid(True, which='both', linestyle='--', alpha=0.5)
    
    # Adjust layout and save
    plt.tight_layout()
    plt.savefig('waveform_with_boundaries.png', dpi=300)
    plt.show()

def main():
    log_file = "udp_packets.log"
    
    print(f"Processing log file: {log_file}")
    samples, sample_rate, metadata = parse_log_file(log_file)
    
    if samples is not None and sample_rate is not None and len(samples) > 0:
        print(f"Found {len(samples)} samples at {sample_rate} Hz")
        print(f"Processed {len(metadata['boundaries'])} packets")
        print("Creating waveform plot with packet boundaries...")
        plot_waveform(samples, sample_rate, metadata)
        save_to_csv(samples, sample_rate, metadata)
        save_to_csv(samples, sample_rate, metadata, minimal=True, filename='waveform_data_minimal.csv')
    else:
        print("No valid wave data found in log file")

if __name__ == "__main__":
    main()
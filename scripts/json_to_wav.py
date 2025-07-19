#!/usr/bin/python3
import json
import numpy as np
import wave
import sys
from scipy.signal import butter, lfilter, medfilt

def debug_print(*args, **kwargs):
    print(*args, **kwargs, file=sys.stderr)
    sys.stderr.flush()

def butter_bandpass(lowcut, highcut, fs, order=5):
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = butter(order, [low, high], btype='band')
    return b, a

def bandpass_filter(data, lowcut, highcut, fs, order=5):
    b, a = butter_bandpass(lowcut, highcut, fs, order=order)
    return lfilter(b, a, data)

def process_packet(byte_data):
    """Enhanced audio processing with multiple noise reduction techniques"""
    seq_num = int.from_bytes(byte_data[:4], 'little')
    audio_data = byte_data[8:]
    samples = np.frombuffer(audio_data, dtype='<i2').astype(np.float32)
    
    # 1. Initial clipping protection
    #samples = np.clip(samples, -32767, 32767)
    
    # 2. Bandpass filter (300Hz-8kHz) to remove extreme low/high frequencies
    #samples = bandpass_filter(samples, 300, 8000, 44100, order=4)
    
    # 3. Adaptive buzzing detection
    median = np.median(samples)
    mad = 1.4826 * np.median(np.abs(samples - median))
    buzzing_mask = np.abs(samples - median) > (7 * mad)
    
    # 4. Selective median filtering only on buzzing samples
    if np.any(buzzing_mask):
        filtered = medfilt(samples, kernel_size=5)
        #samples[buzzing_mask] = filtered[buzzing_mask]
    
    return seq_num, samples.astype(np.int16)

def json_to_wav(json_file, output_wav, sample_rate=44100):
    debug_print(f"Processing {json_file} with enhanced noise reduction...")
    
    with open(json_file) as f:
        data = json.load(f)
    
    all_samples = []
    stats = {
        'total': 0,
        'buzzing': 0,
        'filtered': 0,
        'max_amp': 0
    }
    
    for packet in data:
        if 'data' not in packet:
            continue
            
        try:
            byte_data = bytes.fromhex(packet['data'])
            seq_num, samples = process_packet(byte_data)
            
            stats['total'] += len(samples)
            stats['buzzing'] += np.sum(np.abs(samples) > 25000)
            stats['max_amp'] = max(stats['max_amp'], np.max(np.abs(samples)))
            
            all_samples.extend(samples)
        except Exception as e:
            debug_print(f"Packet error: {e}")
            continue
    
    if not all_samples:
        debug_print("Error: No valid audio")
        return False
    
    # Convert and normalize
    samples_np = np.array(all_samples, dtype=np.float32)
    samples_np -= np.mean(samples_np)  # DC removal
    
    # Smart normalization using 99th percentile
    norm_level = np.percentile(np.abs(samples_np), 99)
    if norm_level > 0:
        samples_np = (samples_np / norm_level) * 0.95 * 32767
    
    samples_int = np.clip(samples_np, -32767, 32767).astype(np.int16)
    
    # Write WAV
    with wave.open(output_wav, 'w') as wav:
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.setframerate(sample_rate)
        wav.writeframes(samples_int.tobytes())
    
    debug_print("\nEnhanced Processing Results:")
    debug_print(f"Total samples processed: {stats['total']}")
    debug_print(f"Buzzing samples detected: {stats['buzzing']} ({stats['buzzing']/stats['total']:.2%})")
    debug_print(f"Maximum amplitude: {stats['max_amp']}/32767")
    debug_print(f"Output duration: {len(samples_int)/sample_rate:.2f} seconds")
    debug_print(f"Output file: {output_wav}")
    
    return True

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: ./json_to_wav.py input.json output.wav")
        sys.exit(1)
    
    json_to_wav(sys.argv[1], sys.argv[2])
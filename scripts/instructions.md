# Build and Run Instructions

Follow these steps to build the project and run the executable for catching udp packets.

## Building the Project

1. **Navigate to the `scripts` directory**:
    ```sh
    cd scripts
    ```

2. **Run the build script**:
    ```sh
    ./build_udp_catch.sh
    ```


## Running the Executable

1. **After building, the executable will be created in the `scripts/build` directory**.
   
2. **Run the executable**:
    ```sh
    sudo ./build/udp_catch
    ```

   The `sudo` command is required to ensure the executable has the necessary permissions.

## Note

- For the program to work, it has to be connected to the AP of the ESP32.

# UDP Waveform Analyzer Python script

This Python script parses UDP packets containing sine wave data from a log file and generates a waveform plot.

## Features

- Parses JSON-formatted log files containing binary wave data
- Concatenates multiple packets into a continuous waveform
- Generates time-scaled plots using the sample rate information
- Handles malformed packets gracefully
- Saves high-resolution PNG images of the waveform

## Prerequisites

- Python 3.8 or later
- Pip package manager

## Installation

1. Clone this repository or download the script files
2. Install the required dependencies:

```bash
pip install -r requirements.txt
```

Then run the `audio_visualize.py`, if you want to see the analyzed images of the json file: 
```bash
./audio_visualize.py
```

Then run the json to wav script to generate a wav file: 
```bash
./json_to_wav.py udp_packets.json output.wav
```

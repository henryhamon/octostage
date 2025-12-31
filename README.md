# OctoStage MIDI Controller

![PlatformIO](https://img.shields.io/badge/PlatformIO-Build-orange)
![Teensy](https://img.shields.io/badge/Hardware-Teensy%202.0-blue)
![License](https://img.shields.io/badge/license-MIT-green)

**Octostage** is a professional-grade, custom-built MIDI foot controller designed for reliability and visual feedback.

It pushes the limits of the **ATmega32U4 (Teensy 2.0)** microcontroller, managing 8 separate OLED displays and a full LED array within a strict **2.5KB RAM** budget through aggressive multiplexing and shared memory strategies. It also features a dedicated **ESP32-C3** co-processor to provide native BLE MIDI support without burdening the main loop.

## 🚀 Key Features

* **Hybrid Connectivity:**
    * **USB MIDI:** Low-latency communication for DAWs.
    * **DIN MIDI (5-Pin):** Classic hardware control (via Serial1).
    * **BLE MIDI:** Wireless control for iOS/Android (via ESP32-C3 bridge).
* **Visual Feedback:**
    * 8x OLED Displays (128x32) providing dynamic labels for each footswitch.
    * 8x RGB LEDs (WS2812b) for state indication (Active/Toggle).
* **Zero-Latency Input:**
    * 10x Footswitches read via a non-blocking scheduler.
    * Optimized debounce algorithms.

## 🛠 Hardware Architecture

The system is built around a dual-MCU topology to separate real-time processing from wireless communication.

### Main Unit (Logic & UI)
* **MCU:** Teensy 2.0 (ATmega32U4 @ 16MHz).
* **Input Mux:** CD74HC4067 (16-channel) reading 10 switches on a single pin.
* **Display Mux:** TCA9548A (I2C Multiplexer) driving 8 SSD1306 OLEDs.
* **Constraint:** The ATmega32U4 has only 2.5KB of RAM. A full frame buffer for 8 screens would require 4KB.
* **Solution:** **"Time-Sliced Shared Buffering"**. The firmware allocates a *single* 512-byte buffer. It renders one screen, switches the I2C channel, sends data, wipes the buffer, and moves to the next dirty screen.

### Wireless Bridge (Comms)
* **MCU:** ESP32-C3 Super Mini.
* **Role:** Acts as a transparent Serial-to-BLE bridge. It receives MIDI messages from the Teensy's Hardware Serial (TX) and broadcasts them over Bluetooth Low Energy.

## 📂 Project Structure (PlatformIO)

This project is structured for **PlatformIO** (VS Code).

```text
src/
├── main.cpp          # Scheduler and Main Loop
├── OctoConfig.h      # Hardware Pinout and Global Settings
├── OctoMux.cpp       # 74HC4067 Driver (Inputs)
├── OctoDisplay.cpp   # TCA9548A + U8g2 Shared Buffer Logic
├── OctoMidi.cpp      # USB / DIN / BLE Routing
└── OctoStorage.cpp   # EEPROM Management (Presets)

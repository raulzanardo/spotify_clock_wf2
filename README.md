# 🎵 Spotify Clock WF2

A real-time display system that shows your currently playing Spotify track on a 64×64 LED matrix, with color-temperature-aware clock animations and optional calendar integration.

## Features

- **Now Playing Display**: Shows album art from your currently playing Spotify track in full color on an LED matrix
- **Adaptive Clock**: Displays time with color temperature that shifts throughout the day (warm at night, cool during day)
- **Calendar Integration** (optional): Displays upcoming calendar events below the clock when idle
- **WiFi Enabled**: Connects to your network for real-time Spotify and time data
- **Spotify OAuth**: Secure authentication with Spotify API for playback state
- **Compact Design**: Optimized for embedded systems (ESP32-based)

## Gallery

### Clock Display
![Clock Display](images/clock.png)

### Album Art Display
![Album Art Display](images/album_art.png)

### Calendar Integration
![Calendar Display](images/calendar.png)

## Hardware Requirements

**Recommended Board:** [HUIDU HD-WF2](https://www.aliexpress.com/w/wholesale-HD-WF2.html) — A compact ESP32-S3 board with built-in HUB75 LED matrix driver interface and GPIO breakout pins.

**Compatible Alternatives:** Any ESP32-S3 microcontroller with enough available GPIO pins (minimum 20+ GPIOs) and USB support can work with this project by configuring the pin mapping in `include/config.h`.

### Core Requirements

- **ESP32-S3** microcontroller with USB support and 20+ available GPIO pins
- **HUB75 LED Matrix Driver** (onboard on HD-WF2, or external daughterboard)
- **64×64 RGB LED Matrix Panel** (HUB75 interface, standard module that use nornal shift-registers, the S-PWM are not supported by the lib yet)
- WiFi connectivity (built-in to ESP32-S3)

### Pin Configuration (WF2)

The project uses the WF2 pin mapping for the HUIDU HD-WF2 board's HUB75 LED matrix driver interface:

```
Color Pins (Port X1):
  R1: GPIO 10, R2: GPIO 11
  G1: GPIO 6,  G2: GPIO 7
  B1: GPIO 2,  B2: GPIO 3
  E:  GPIO 21

Control Pins:
  A: GPIO 39,  B: GPIO 38,  C: GPIO 37,  D: GPIO 36
  OE: GPIO 35, CLK: GPIO 34, LAT: GPIO 33
```

## Software Dependencies

- PlatformIO
- ESP32-HUB75-MatrixPanel-I2S-DMA
- SpotifyEsp32
- LittleFS (for image caching)
- JPEGDEC (for album art rendering)
- Arduino framework

See `platformio.ini` for the full dependency list.

## Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/raulzanardo/spotify_clock_wf2.git
cd spotify_clock_wf2
```

### 2. Create Configuration File

Copy the example configuration and fill in your credentials:

```bash
cp include/config.example.h include/config.h
```

### 3. Configure Spotify Credentials

Edit `include/config.h` and add:

- **CLIENT_ID** & **CLIENT_SECRET**: Register your app at [Spotify Developer Dashboard](https://developer.spotify.com/dashboard)
- **REFRESH_TOKEN**: Obtained after first successful authentication (the device will display instructions)
- **WIFI_SSID** & **WIFI_PASS**: Your WiFi network credentials
- **CALENDAR_URL** (optional): HTTP endpoint that returns newline-separated calendar events

### 4. Build, Upload & Monitor

Use the PlatformIO buttons in the VS Code extension to build, upload and open the serial monitor (bottom bar / status bar). This provides GUI actions for "Build", "Upload" and "Monitor".

## Color Temperature Algorithm

The clock color shifts throughout the day based on Kelvin temperature:

- **Night (10 PM - 6 AM)**: 1500K warm light, dimmed to 30% brightness
- **Morning (6 AM - 12 PM)**: Gradually increases from 2000K to 6500K
- **Afternoon (12 PM - 6 PM)**: Gradually decreases from 6500K back to 2000K
- **Evening (6 PM - 10 PM)**: Cools down from 2000K toward night temp

This creates a natural, soothing display that adapts to your circadian rhythm.

## Calendar Integration

You can enable it in the config file: `include/config.h`

```cpp
// Calendar display (uncomment to enable)
// #define ENABLE_CALENDAR
```


If enabled, the device fetches calendar data from a configurable HTTP endpoint every 10 seconds when Spotify is idle. The endpoint should return plain text with events separated by newlines.

Example calendar provider script in `docs/calendar.example.sh` (included).

This was done in another device to reduce the RAM usage in the ESP, in my case I added this script to my local server that runs Debian and Apatch configured with cgi-bin with some bash scripts. 

## File Structure

```
src/main.cpp              # Main firmware code
include/config.h          # User configuration (keep private!)
include/config.example.h  # Configuration template
docs/                     # Documentation and helper scripts
  └── calendar.example.sh # Calendar script template
platformio.ini           # PlatformIO configuration
```

## Troubleshooting

### Board Not Found
Ensure your ESP32 is connected via USB. Check if you have the CH340 driver installed (common on budget ESP32 boards).

### Spotify Authentication Fails
- Verify CLIENT_ID and CLIENT_SECRET are correct
- Check that your WiFi is connected (watch serial output)
- Ensure the device can reach `api.spotify.com`

### LED Matrix Not Lighting
- Check all HUB75 pin connections match WF2 configuration
- Verify matrix power supply is adequate (may need external 5V)
- Try adjusting `setBrightness8()` in setup (currently set to 30)

### No Calendar Events Showing
- Verify CALENDAR_URL is correctly formatted in config.h
- Check that your calendar endpoint returns proper newline-separated text
- Ensure the device can reach the calendar URL (test from the same network)

## Performance Notes

- Album art is downloaded and cached in LittleFS (reduces bandwidth)
- Spotify state is checked every 4 seconds
- Calendar is refreshed every 10 seconds (when music is idle)
- Color temperature calculation is done in integer math where possible

## License

[Add your license here]

## Contributing

Contributions welcome! Please ensure:
- No secrets are committed (use `include/config.h` locally)
- Code follows existing style (const correctness, USBSerial debugging)
- Changes are tested on real hardware before PR

## Libraries & Resources

### Display & Graphics
- [ESP32-HUB75-MatrixPanel-I2S-DMA](https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA) — HUB75 LED matrix driver library
- [JPEGDEC](https://github.com/bitbank2/JPEGDEC) — JPEG decoder for embedded systems

### API & Connectivity
- [SpotifyEsp32](https://github.com/jetcitycomet/SpotifyEsp32) — Spotify API client library for ESP32

### External APIs
- [Spotify Web API](https://developer.spotify.com/documentation/web-api) — Spotify playback state and metadata

## Inspiration & Credits

**Project inspired by:**
- [64x64 RGB LED Matrix Album Art Display on Pi 3B+](https://www.reddit.com/r/raspberry_pi/comments/ziz4hk/my_64x64_rgb_led_matrix_album_art_display_pi_3b/) — Original concept by the Raspberry Pi community
- [Live Spotify Album Art Display Tutorial](https://learn.sparkfun.com/tutorials/live-spotify-album-art-display/all) — SparkFun's guide to album art displays

**Built with help from:**
- [mrcodetastic's HD-WF1-WF2-LED-MatrixPanel-DMA](https://github.com/mrcodetastic/HD-WF1-WF2-LED-MatrixPanel-DMA) — Core driver library
- [ESP32-HUB75-MatrixPanel-DMA discussion](https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA/issues/433) — Troubleshooting and optimization insights
- [Eltopinovatif's HD-WF2 v7 implementation](https://github.com/Eltopinovatif/HD-WF2_v7-JWS-Assalaam) — Reference implementation for WF2 board

---

**Enjoy your Spotify Clock!** 🎵✨

# 🎵 Spotify Clock WF2

A real-time display system that shows your currently playing Spotify track on a 64×64 LED matrix, with beautiful color-temperature-aware clock animations and optional calendar integration.

## Features

- **Now Playing Display**: Shows album art from your currently playing Spotify track in full color on an LED matrix
- **Adaptive Clock**: Displays time with color temperature that shifts throughout the day (warm at night, cool during day)
- **Calendar Integration** (optional): Displays upcoming calendar events below the clock when idle
- **WiFi Enabled**: Connects to your network for real-time Spotify and time data
- **Spotify OAuth**: Secure authentication with Spotify API for playback state
- **Compact Design**: Optimized for embedded systems (ESP32-based)

## Hardware Requirements

- **ESP32** microcontroller with USB support
- **WF2 LED Matrix Driver** with specific pin configuration
- **64×64 RGB LED Matrix Panel** (HUB75 interface)
- WiFi connectivity

### Pin Configuration (WF2)

The project uses a custom WF2 pin mapping for the HUB75 LED matrix driver:

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

### 4. Build and Upload

```bash
platformio run --target upload
```

### 5. Monitor Serial Output

```bash
platformio device monitor
```

## Features Configuration

You can disable certain Spotify API features to reduce memory usage:

```cpp
// In include/config.h, uncomment to disable:
#define DISABLE_AUDIOBOOKS
#define DISABLE_CATEGORIES
#define DISABLE_CHAPTERS
#define DISABLE_EPISODES
#define DISABLE_GENRES
#define DISABLE_MARKETS
#define DISABLE_PLAYLISTS
#define DISABLE_SEARCH
#define DISABLE_SHOWS

// Calendar display (commented = enabled)
// #define DISABLE_CALENDAR
```

## Color Temperature Algorithm

The clock color shifts throughout the day based on Kelvin temperature:

- **Night (10 PM - 6 AM)**: 1500K warm light, dimmed to 30% brightness
- **Morning (6 AM - 12 PM)**: Gradually increases from 2000K to 6500K
- **Afternoon (12 PM - 6 PM)**: Gradually decreases from 6500K back to 2000K
- **Evening (6 PM - 10 PM)**: Cools down from 2000K toward night temp

This creates a natural, soothing display that adapts to your circadian rhythm.

## Calendar Integration

If enabled, the device fetches calendar data from a configurable HTTP endpoint every 10 seconds when Spotify is idle. The endpoint should return plain text with events separated by newlines.

Example calendar provider script in `docs/calendar.sh` (included).

## File Structure

```
src/main.cpp              # Main firmware code
include/config.h          # User configuration (keep private!)
include/config.example.h  # Configuration template
docs/                     # Documentation and helper scripts
  ├── calendar.sh        # Calendar fetch script
  ├── calendar.example.sh # Calendar script template
  └── response_playing.json # Example Spotify API response
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

---

**Enjoy your Spotify Clock!** 🎵✨

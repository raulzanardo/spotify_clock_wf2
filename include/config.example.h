#ifndef SPOTIFY_CLOCK_CONFIG_H
#define SPOTIFY_CLOCK_CONFIG_H

#define DISABLE_AUDIOBOOKS
#define DISABLE_CATEGORIES
#define DISABLE_CHAPTERS
#define DISABLE_EPISODES
#define DISABLE_GENRES
#define DISABLE_MARKETS
#define DISABLE_PLAYLISTS
#define DISABLE_SEARCH
#define DISABLE_SHOWS

// Enable calendar display by defining `ENABLE_CALENDAR` below.
// Uncomment to enable calendar support:
// #define ENABLE_CALENDAR

#define WF2_X1_R1_PIN 10 // in the smaller one the R B are changed
#define WF2_X1_R2_PIN 11
#define WF2_X1_G1_PIN 6
#define WF2_X1_G2_PIN 7
#define WF2_X1_B1_PIN 2
#define WF2_X1_B2_PIN 3
#define WF2_X1_E_PIN 21

#define WF2_X2_R1_PIN 4
#define WF2_X2_R2_PIN 5
#define WF2_X2_G1_PIN 8
#define WF2_X2_G2_PIN 9
#define WF2_X2_B1_PIN 12
#define WF2_X2_B2_PIN 13
#define WF2_X2_E_PIN -1 // Currently unknown, so X2 port will not work (yet) with 1/32 scan panels

#define WF2_A_PIN 39
#define WF2_B_PIN 38
#define WF2_C_PIN 37
#define WF2_D_PIN 36
#define WF2_OE_PIN 35
#define WF2_CLK_PIN 34
#define WF2_LAT_PIN 33

#define WF2_PINS {WF2_X1_R1_PIN, WF2_X1_G1_PIN, WF2_X1_B1_PIN, WF2_X1_R2_PIN, WF2_X1_G2_PIN, WF2_X1_B2_PIN, WF2_A_PIN, WF2_B_PIN, WF2_C_PIN, WF2_D_PIN, WF2_X1_E_PIN, WF2_LAT_PIN, WF2_OE_PIN, WF2_CLK_PIN}

#define WF2_BUTTON_TEST 17 // Test key button on PCB, 1=normal, 0=pressed
#define WF2_LED_RUN_PIN 40 // Status LED on PCB
#define WF2_USB_DM_PIN 19
#define WF2_USB_DP_PIN 20

#define PANEL_RES_X 64
#define PANEL_RES_Y 64

#define WIFI_SSID ""
#define WIFI_PASS ""

#define CALENDAR_URL ""

#define PROJECTNAME "spotify_clock_wf2"

#define CLIENT_ID ""
#define CLIENT_SECRET ""
#define REFRESH_TOKEN ""

// ===== TIME SETTINGS =====
#define ntpServer1 "pool.ntp.org"
#define ntpServer2 "time.nist.gov"

// Timezone: BRT3 is Brasília Time (UTC-3, no DST)
// Other examples: PST8PDT, EST5EDT, CST6CDT, GMT0, CET-1CEST, etc.
#define TIME_ZONE "BRT3"

// UTC Offset in seconds (BRT3 = UTC-3 = -10800 seconds)
// Can also be set as: UTC offset hours * 3600 + minutes * 60
#define UTC_OFFSET_SECONDS -10800

// ===== COLOR TEMPERATURE SETTINGS =====
// Night time hour range (0-23 format)
#define CONFIG_NIGHT_START_HOUR 22  // 10 PM
#define CONFIG_NIGHT_END_HOUR 6     // 6 AM

// Color temperature in Kelvin
#define CONFIG_NIGHT_TEMP 1500.0f    // Very warm light for night
#define CONFIG_MIN_TEMP 2000.0f      // Warm light (sunset/sunrise)
#define CONFIG_MAX_TEMP 6500.0f      // Cool light (midday)

// Nighttime brightness dimming factor (0.0 to 1.0)
#define CONFIG_NIGHT_DIM_FACTOR 0.3f

#endif // SPOTIFY_CLOCK_CONFIG_H

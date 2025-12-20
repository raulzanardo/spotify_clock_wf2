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

#define PANEL_RES_X 64 // Number of pixels wide of each INDIVIDUAL panel module.
#define PANEL_RES_Y 64 // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1  // Total number of panels chained one to another

#define WIFI_SSID ""
#define WIFI_PASS ""

#define CALENDAR_URL ""

#define PROJECTNAME "spotify_clock_wf2"

#define CLIENT_ID ""
#define CLIENT_SECRET ""
#define REFRESH_TOKEN ""

#define ntpServer1 "pool.ntp.org"
#define ntpServer2 "time.nist.gov"

#endif // SPOTIFY_CLOCK_CONFIG_H

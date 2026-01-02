#include <config.h>
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#include <WiFi.h>
#include <SpotifyEsp32.h>
#include "FS.h"
#include <LittleFS.h>
#include <ESPmDNS.h>
#include <time.h>
#include <HTTPClient.h>
#include <JPEGDEC.h>

#include <Fonts/Picopixel.h>
#include <Fonts/FreeSans12pt7b.h>

#define countof(x) (sizeof(x) / sizeof(x[0]))
#define SETUP_LOG_LINES 8

// Function prototypes
uint16_t getClockDigitColor(int hour, int minute);
void drawClock(const String &clockText, uint16_t bodyColor, int yOffset = 39);
bool hasInternetConnectivity();
void ensureSpotifyReady();
void addSetupLog(const String &msg);
void drawSetupLogs();

#ifdef ENABLE_CALENDAR
String fetchCalendar();
int measureTextHeight(const String &text, const GFXfont *font);
int countLines(const String &text);
void drawCalendarLines(const String &calendarText, uint16_t bodyColor, int startY, int lineHeight);
#endif
int downloadImage(const String &imageUrl);
int drawMCU(JPEGDRAW *pDraw);
void drawJPEG(const char *filename, int xpos, int ypos);

///////////////////////////////////////////////////////////////////////////////////
// MatrixPanel_I2S_DMA dma_display;
MatrixPanel_I2S_DMA *dma_display = nullptr;

String currentAlbumArtUrl = "";
String previousAlbumArtUrl = " ";
bool isSpotifyPlaying = false;
bool spotifyInitialized = false;
bool spotifyAuthenticated = false;
String setupLogs[SETUP_LOG_LINES];
int setupLogCount = 0;
#ifdef ENABLE_CALENDAR
unsigned long lastCalendarFetch = 0;
String lastCalendarResponse = "";
#endif
///////////////////////////////////////////////////////////////////////////////////

Spotify sp(CLIENT_ID, CLIENT_SECRET, REFRESH_TOKEN, true);
JPEGDEC jpeg;

struct tm timeinfo;
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
uint16_t getClockDigitColor(int hour, int minute)
{

    // Calculate the time as a float from 0 to 24
    float timeOfDay = hour + minute / 60.0f;

    // Calculate color temperature
    float temp;
    if (timeOfDay < CONFIG_NIGHT_END_HOUR || timeOfDay >= CONFIG_NIGHT_START_HOUR)
    {
        // Night time (10 PM to 6 AM)
        temp = NIGHT_TEMP;
    }
    else if (timeOfDay < 12)
    {
        // Morning: temperature increases
        temp = MIN_TEMP + (MAX_TEMP - MIN_TEMP) * ((timeOfDay - 6) / 6.0f);
    }
    else if (timeOfDay < 18)
    {
        // Afternoon: temperature decreases
        temp = MAX_TEMP - (MAX_TEMP - MIN_TEMP) * ((timeOfDay - 12) / 6.0f);
    }
    else
    {
        // Evening: temperature decreases to night temp
        temp = MIN_TEMP - (MIN_TEMP - NIGHT_TEMP) * ((timeOfDay - 18) / 4.0f);
    }

    // Convert temperature to RGB
    float red, green, blue;

    // Approximation of RGB values from color temperature
    // Based on a simplified version of the algorithm by Tanner Helland
    temp = temp / 100;

    if (temp <= 66)
    {
        red = 255;
        green = 99.4708025861f * std::log(temp) - 161.1195681661f;
        if (temp <= 19)
        {
            blue = 0;
        }
        else
        {
            blue = 138.5177312231f * std::log(temp - 10) - 305.0447927307f;
        }
    }
    else
    {
        red = 329.698727446f * std::pow(temp - 60, -0.1332047592f);
        green = 288.1221695283f * std::pow(temp - 60, -0.0755148492f);
        blue = 255;
    }

    // Clamp RGB values to 0-255 range
    red = std::min(255.0f, std::max(0.0f, red));
    green = std::min(255.0f, std::max(0.0f, green));
    blue = std::min(255.0f, std::max(0.0f, blue));

    // Dim the color at night
    if (timeOfDay < CONFIG_NIGHT_END_HOUR || timeOfDay >= CONFIG_NIGHT_START_HOUR)
    {
        float dimFactor = CONFIG_NIGHT_DIM_FACTOR; // Adjust this value to change nighttime brightness
        red *= dimFactor;
        green *= dimFactor;
        blue *= dimFactor;
    }

    // Convert to RGB565
    uint16_t r = static_cast<uint16_t>(red * 31 / 255);
    uint16_t g = static_cast<uint16_t>(green * 63 / 255);
    uint16_t b = static_cast<uint16_t>(blue * 31 / 255);

    return (r << 11) | (g << 5) | b;
}

#ifdef ENABLE_CALENDAR
String fetchCalendar()
{
    HTTPClient http;
    String response = "";

    USBSerial.println(F("Fetching calendar..."));
    http.begin(CALENDAR_URL);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
        response = http.getString();
        USBSerial.println("Calendar response: " + response);
    }
    else
    {
        USBSerial.println("[HTTP] Calendar GET failed, error: " + http.errorToString(httpCode) + " : " + String(httpCode));
    }

    http.end();
    return response;
}

int measureTextHeight(const String &text, const GFXfont *font)
{
    dma_display->setFont(font);
    dma_display->setTextSize(1);
    dma_display->setTextWrap(false);

    int16_t x1, y1;
    uint16_t w, h;
    dma_display->getTextBounds(text.c_str(), 0, 0, &x1, &y1, &w, &h);
    return h;
}

int countLines(const String &text)
{
    if (text.length() == 0)
        return 0;

    int count = 1;
    int start = 0;
    while (start >= 0 && start < text.length())
    {
        int nl = text.indexOf('\n', start);
        if (nl == -1)
            break;
        count++;
        start = nl + 1;
    }
    return count;
}

void drawCalendarLines(const String &calendarText, uint16_t bodyColor, int startY, int lineHeight)
{
    dma_display->setTextSize(1);
    dma_display->setTextWrap(false);
    dma_display->setFont(&Picopixel);
    dma_display->setTextColor(bodyColor);

    int cursorY = startY;
    int start = 0;
    while (start <= calendarText.length())
    {
        int nl = calendarText.indexOf('\n', start);
        String line = (nl == -1) ? calendarText.substring(start) : calendarText.substring(start, nl);

        dma_display->setCursor(1, cursorY);
        dma_display->printf("%s", line.c_str());

        if (nl == -1)
            break;
        start = nl + 1;
        cursorY += lineHeight;
    }
}
#endif

void drawClock(const String &clockText, uint16_t bodyColor, int yOffset)
{
    int xOffset = 3;

    dma_display->setTextSize(1);
    dma_display->setTextWrap(false);
    dma_display->setFont(&FreeSans12pt7b);

    dma_display->setCursor(xOffset, yOffset);
    dma_display->setTextColor(bodyColor);
    dma_display->printf("%s", clockText.c_str());
}

bool hasInternetConnectivity()
{
    HTTPClient http;
    http.setConnectTimeout(3000);
    http.setTimeout(3000);

    // Lightweight connectivity check endpoint
    if (!http.begin("http://clients3.google.com/generate_204"))
    {
        USBSerial.println(F("Connectivity check begin failed"));
        return false;
    }

    int code = http.GET();
    http.end();
    bool ok = code == 204;
    if (ok)
    {
        USBSerial.println(F("Internet reachable"));
    }
    else
    {
        USBSerial.println("No internet, code: " + String(code));
    }
    return ok;
}

void ensureSpotifyReady()
{
    if (spotifyAuthenticated)
        return;

    if (WiFi.status() != WL_CONNECTED)
    {
        USBSerial.println(F("WiFi lost, cannot init Spotify"));
        spotifyInitialized = false;
        return;
    }

    if (!hasInternetConnectivity())
    {
        USBSerial.println(F("No internet, deferring Spotify auth"));
        return;
    }

    if (!spotifyInitialized)
    {
        USBSerial.print(F("Spotify begin: "));
        sp.begin();
        spotifyInitialized = true;
        USBSerial.println(F("started"));
    }

    if (!sp.is_auth())
    {
        USBSerial.print(F("Authenticating Spotify (timeout 10s): "));
        unsigned long start = millis();
        while (!sp.is_auth() && millis() - start < 10000)
        {
            sp.handle_client();
            delay(10);
        }
        if (sp.is_auth())
        {
            spotifyAuthenticated = true;
            USBSerial.printf("Authenticated! Refresh token: %s\n", sp.get_user_tokens().refresh_token);
        }
        else
        {
            USBSerial.println(F("Auth not completed, will retry later"));
        }
    }
    else
    {
        spotifyAuthenticated = true;
        USBSerial.println(F("Spotify already authenticated"));
    }
}

void addSetupLog(const String &msg)
{
    if (setupLogCount < SETUP_LOG_LINES)
    {
        setupLogs[setupLogCount++] = msg;
    }
    else
    {
        // shift up to keep the latest logs
        for (int i = 1; i < SETUP_LOG_LINES; ++i)
        {
            setupLogs[i - 1] = setupLogs[i];
        }
        setupLogs[SETUP_LOG_LINES - 1] = msg;
    }
    drawSetupLogs();
}

void drawSetupLogs()
{
    if (dma_display == nullptr)
        return;

    dma_display->clearScreen();
    dma_display->setFont(&Picopixel);
    dma_display->setTextSize(1);
    dma_display->setTextWrap(false);
    dma_display->setTextColor(0xFFFF); // white

    const int lineHeight = 8;
    for (int i = 0; i < setupLogCount; ++i)
    {
        int y = (i + 1) * lineHeight;
        dma_display->setCursor(0, y);
        dma_display->printf("%s", setupLogs[i].c_str());
    }
    dma_display->flipDMABuffer();
}

int downloadImage(const String &imageUrl)
{
    USBSerial.println("Downloading image... " + imageUrl);
    HTTPClient http;

    File f = LittleFS.open("/cover.jpg", "w");

    if (!f)
    {
        USBSerial.println("Error opening file");
        return -1;
    }

    http.begin(imageUrl);

    int httpCode = http.GET();

    if (httpCode != HTTP_CODE_OK)
    {
        USBSerial.println("[HTTP] GET... failed, error: " + http.errorToString(httpCode) + " : " + String(httpCode));
        USBSerial.println("Response: " + http.getString());
        f.close();
        http.end();
        return -1;
    }

    int fileCode = http.writeToStream(&f);

    if (fileCode < 0)
    {
        USBSerial.println(F("Error writing to file"));
        f.close();
        http.end();
        return -1;
    }

    USBSerial.println("File Downloaded");

    f.close();
    http.end();
    return 0;
}

int drawMCU(JPEGDRAW *pDraw)
{
    uint16_t *pPixel = (uint16_t *)pDraw->pPixels;
    for (int y = 0; y < pDraw->iHeight; y++)
    {
        for (int x = 0; x < pDraw->iWidth; x++)
        {
            dma_display->drawPixel(x + pDraw->x, y + pDraw->y, pPixel[y * pDraw->iWidth + x]);
        }
    }
    return 1; // Continue decoding
}

void drawJPEG(const char *filename, int xpos, int ypos)
{
    File file = LittleFS.open(filename, "r");
    if (!file)
    {
        USBSerial.println("Failed to open file for reading");
        return;
    }

    int fileSize = file.size();
    uint8_t *buffer = (uint8_t *)malloc(fileSize);
    if (!buffer)
    {
        USBSerial.println("Not enough memory to load image");
        file.close();
        return;
    }

    file.read(buffer, fileSize);
    file.close();

    if (jpeg.openRAM(buffer, fileSize, drawMCU))
    {
        jpeg.decode(xpos, ypos, 0); // 0 = full size
        jpeg.close();
    }

    free(buffer);
}

void setup()
{
    // Initialize USBSerial port for debugin
    USBSerial.begin(115200);
    USBSerial.println("\nSTART!");
    addSetupLog("Booting...");

    // Initialize led matrix
    USBSerial.println(F("Led Matrix begin"));
    HUB75_I2S_CFG mxconfig(
        64,
        64,
        1,
        WF2_PINS);
    mxconfig.driver = HUB75_I2S_CFG::ICN2038S;
    mxconfig.clkphase = false;
    mxconfig.latch_blanking = 4;
    mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_10M;
    mxconfig.double_buff = true;

    // Display Setup
    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    dma_display->begin();
    dma_display->setBrightness8(30);
    dma_display->clearScreen();
    dma_display->flipDMABuffer();
    addSetupLog("Display ready");

    // Initialize LittleFS
    USBSerial.print(F("LittleFS begin: "));
    if (!LittleFS.begin(true))
    {
        USBSerial.println(F("failed"));
        addSetupLog("FS: failed");
    }
    else
    {
        USBSerial.println(F("ok"));
        addSetupLog("FS: ok");
    }

    // Initialize Wifi
    USBSerial.print(F("WiFi begin: "));
    addSetupLog("WiFi: connecting...");

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        USBSerial.print(".");
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        USBSerial.println(F("\nWiFi failed! Restarting..."));
        addSetupLog("WiFi: failed");
        // ESP.restart();
    }
    else
    {
        USBSerial.println((String) "RSSI : " + WiFi.RSSI() + " dB");
        USBSerial.println("\nIP:" + WiFi.localIP().toString());
        addSetupLog("WiFi: connected");
        addSetupLog("IP: " + WiFi.localIP().toString());
    }

    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    // Initialize mDNS
    USBSerial.print(F("mDNS begin: "));
    if (!MDNS.begin(PROJECTNAME))
    {
        USBSerial.println(F("failed"));
        addSetupLog("mDNS: failed");
        // ESP.restart();
    }
    else
    {
        // Set the hostname to "$PROJECTNAME.local"
        USBSerial.println(F("ok"));
        addSetupLog("mDNS: ok");
    }

    // Initialize NTPC
    setenv("TZ", TIME_ZONE, 1);                       // Set timezone
    configTime(UTC_OFFSET_SECONDS, 0, ntpServer1, ntpServer2); // Init and get the time

    if (!getLocalTime(&timeinfo))
    {
        USBSerial.println(F("Failed to obtain time"));
        addSetupLog("Time: failed");
        return;
    }
    USBSerial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    addSetupLog("Time: synced");

    ensureSpotifyReady(); // Will defer if no internet
    addSetupLog(spotifyAuthenticated ? "Spotify: ok" : "Spotify: pending");
}

void loop()
{
    dma_display->clearScreen();
    //-------------------------------------------------------------------------------
    ensureSpotifyReady();

    if (!spotifyAuthenticated)
    {
        USBSerial.println(F("Spotify not ready, showing clock only"));

        if (!getLocalTime(&timeinfo))
        {
            USBSerial.println(F("Failed to obtain time"));
        }

        char datestring[6];
        snprintf_P(datestring,
                   countof(datestring),
                   PSTR("%02u:%02u"),
                   timeinfo.tm_hour,
                   timeinfo.tm_min);

        uint16_t bodyColor = getClockDigitColor(timeinfo.tm_hour, timeinfo.tm_min);
        drawClock(datestring, bodyColor, 39);
        dma_display->flipDMABuffer();
        delay(4000);
        return;
    }

    USBSerial.println(F("Checking Spotify state"));

    response currentState = sp.currently_playing();
    USBSerial.println();
    USBSerial.println(currentState.reply.as<String>());
    USBSerial.println();

    /*
    State
      200 Information about playback
      204 Playback not available or active
      401 Bad or expired token. This can happen if the user revoked a token or the access token has expired. You should re-authenticate the user.
      403 Bad OAuth request (wrong consumer key, bad nonce, expired timestamp...). Unfortunately, re-authenticating the user won't help here.
      429 The app has exceeded its rate limits.
    */

    if (currentState.status_code != 200)
    {
        USBSerial.println("Error, code: " + String(currentState.status_code));

        if (currentState.status_code == 201)
        {
            USBSerial.println(F("Spotify on inactive"));

            isSpotifyPlaying = false;
        }

        if (currentState.status_code == 401)
        {
            USBSerial.println(F("The access token expired"));

            sp.get_access_token();
            currentState = sp.currently_playing();
        }

        if (currentState.status_code == 403)
        {
            USBSerial.println(F("Bad OAuth request"));
        }

        if (currentState.status_code == 429)
        {
            USBSerial.println(F("The app has exceeded its rate limits."));
        }

        if (currentState.reply["message"].as<String>().equals("Timeout receiving headers"))
        {
            USBSerial.println(F("Timeout receiving headers"));
            currentState = sp.currently_playing();
        }
    }

    // check if is play is null
    if (!currentState.reply["is_playing"].isNull())
    {
        isSpotifyPlaying = currentState.reply["is_playing"].as<bool>();
    }

    if (isSpotifyPlaying)
    {
        USBSerial.println(F("Spotify is playing"));
        currentAlbumArtUrl = currentState.reply["item"]["album"]["images"][2]["url"].as<String>();

        if (!currentState.reply["item"]["album"]["images"][2]["url"].isNull())
        {

            if (!currentAlbumArtUrl.equals(previousAlbumArtUrl))
            {
                previousAlbumArtUrl = currentAlbumArtUrl;
                int downloadResult = downloadImage(currentAlbumArtUrl);

                USBSerial.println("Download result: " + String(downloadResult));
            }
        }

        drawJPEG("/cover.jpg", 0, 0);
    }
    else
    {
        USBSerial.println(F("Spotify is not playing, drawing clock"));

        if (!getLocalTime(&timeinfo))
        {
            USBSerial.println(F("Failed to obtain time"));
        }

        char datestring[6];
        snprintf_P(datestring,
                   countof(datestring),
                   PSTR("%02u:%02u"),
                   timeinfo.tm_hour,
                   timeinfo.tm_min);

        uint16_t bodyColor = getClockDigitColor(timeinfo.tm_hour, timeinfo.tm_min);
        bool hasCalendar = false;

#ifdef ENABLE_CALENDAR
        unsigned long now = millis();
        if (now - lastCalendarFetch >= 10000 || lastCalendarFetch == 0)
        {
            lastCalendarResponse = fetchCalendar();
            lastCalendarFetch = now;
        }

        hasCalendar = lastCalendarResponse.length() > 0;
#endif

#ifdef ENABLE_CALENDAR
        if (hasCalendar)
        {
            const int panelHeight = PANEL_RES_Y;
            int clockHeight = measureTextHeight(datestring, &FreeSans12pt7b);
            int calendarLineHeight = measureTextHeight("A", &Picopixel) + 2; // add 2px spacing between lines
            int calendarLines = countLines(lastCalendarResponse);

            int contentHeight = clockHeight + calendarLineHeight * calendarLines;
            int availableHeight = panelHeight; // panel height

            // space-around: equal space around each block; top/bottom get half the between space
            int remaining = max(0, availableHeight - contentHeight);
            // For two blocks, spacing_between = remaining / 2, top = bottom = spacing_between / 2
            float spacingBetween = remaining / 2.0f;
            float edgeSpacing = spacingBetween / 2.0f;

            int clockY = static_cast<int>(edgeSpacing + clockHeight + 0.5f);
            int calendarStartY = static_cast<int>(clockY + spacingBetween + calendarLineHeight + 0.5f);

            drawClock(datestring, bodyColor, clockY);
            drawCalendarLines(lastCalendarResponse, bodyColor, calendarStartY, calendarLineHeight);
        }
        else
        {
            drawClock(datestring, bodyColor, 39);
        }
#else
        // Calendar feature disabled: just draw the clock centered at default Y
        drawClock(datestring, bodyColor, 39);
#endif

        currentAlbumArtUrl = "";
        previousAlbumArtUrl = " ";
    }
    //-------------------------------------------------------------------------------
    dma_display->flipDMABuffer();

    delay(4000);
}
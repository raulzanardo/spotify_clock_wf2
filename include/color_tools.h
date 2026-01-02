// Color utilities for clock digits
#pragma once

#include <config.h>
#include <cmath>
#include <algorithm>

static inline uint16_t getClockDigitColor(int hour, int minute)
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

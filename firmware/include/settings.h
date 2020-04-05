#pragma once

struct Settings
{
    int32_t calibrationSlope = 0;
    uint8_t logCadence = 0;

    static Settings *get();
    static void save();
};

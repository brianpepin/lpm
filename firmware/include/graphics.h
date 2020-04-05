#pragma once

#include <U8g2lib.h>

// Global variables
extern U8G2_SSD1309_128X64_NONAME0_1_4W_HW_SPI display;

namespace Graphics
{
    namespace GraphBuffer
    {
        constexpr uint8_t max = 128;
        extern int32_t buffer[max];
        extern void* owner;
    }

    void drawDottedHLine(uint8_t x, uint8_t y, uint8_t len);
    void drawDottedVLine(uint8_t x, uint8_t y, uint8_t len);
    void printPROGMEM(const char* text);
}

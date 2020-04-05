#include <globals.h>
#include <graphics.h>

U8G2_SSD1309_128X64_NONAME0_1_4W_HW_SPI display(U8G2_R0, PIN_DISPLAY_CS, PIN_DISPLAY_DC, PIN_DISPLAY_RST);

namespace Graphics
{
    namespace GraphBuffer
    {
        int32_t buffer[max];
        void* owner;
    }

    void drawDottedHLine(uint8_t x, uint8_t y, uint8_t len)
    {
        for (uint8_t px = x; px < x + len; px += 2)
        {
            display.drawPixel(px, y);
        }
    }

    void drawDottedVLine(uint8_t x, uint8_t y, uint8_t len)
    {
        for (uint8_t py = y; py < y + len; py += 2)
        {
            display.drawPixel(x, py);
        }
    }

    void printPROGMEM(const char* text)
    {
        for (size_t c = 0;;c++)
        {
            char ch = (char)pgm_read_byte_near(text + c);
            if (!ch) break;
            display.print(ch);
        }        
    }
}

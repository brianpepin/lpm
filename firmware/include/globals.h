
#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <avr/pgmspace.h>

//
// Display port map
//

// DISP_CS:    PC0 pin A0
// DISP_RST:   PC1 pin A1 
// DISP_DC:    PC2 pin A2

#define PIN_DISPLAY_CS    A0
#define PIN_DISPLAY_RST   A1
#define PIN_DISPLAY_DC    A2

//
// AD port map
//

// ADC_DRDY     PB0 pin 8
// ADC_SCK      PB1 pin 9
// ADC_PWR      PB2 pin 10
// ADC_TEMP     PC3 pin A3

#define PIN_ADC_DRDY 8
#define PIN_ADC_SCK  9
#define PIN_ADC_PWR  10
#define PIN_ADC_TEMP A3

//
// D-Pad port map
//

// PWR_CPU      PD7 pin 7
// RIGHT        PD6 pin 6
// LEFT         PD5 pin 5
// DOWN         PD4 pin 4
// UP           PD3 pin 3
// SELECT       PD2 pin 2

#define PIN_PWR_CPU 7
#define PIN_RIGHT   6
#define PIN_LEFT    5
#define PIN_DOWN    4
#define PIN_UP      3
#define PIN_SELECT  2

//
// Battery status port map
//

// CHG      PC5     A5
// V_BAT    ADC7    A7

#define PIN_CHG     A5
#define PIN_V_BAT   A7

//
// Flash Memory port map
//

// FLASH_CS PC4 A4

#define PIN_FLASH_CS A4

// Macros

 #define COUNT_OF(a) (sizeof(a) / sizeof(*a))

// FONTS
#define MENU_HEADER_FONT        u8g2_font_profont11_tr
#define MENU_ITEM_FONT          u8g2_font_profont11_tr
#define MENU_ARROW_FONT         u8g2_font_open_iconic_arrow_1x_t
#define NUMBER_SELECTOR_FONT    u8g2_font_profont17_tn
#define NOTICE_FONT             u8g2_font_haxrcorp4089_tr
#define MAIN_READING_FONT       u8g2_font_profont29_tn
#define MAIN_READING_FONT_SM    u8g2_font_profont22_tn
#define MAIN_UNITS_FONT         u8g2_font_tenthinnerguys_tr
#define GRAPH_READING_FONT      u8g2_font_blipfest_07_tr
#define LOGGING_INDICATOR_FONT  u8g2_font_blipfest_07_tr

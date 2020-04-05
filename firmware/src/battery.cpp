#include <globals.h>
#include <battery.h>
#include <graphics.h>

#define CHARGE_ANIMATION_FRAME_TICKS 500

// LiPo max charge = 4.2v. There are five ticks on our
// graph so the first tick should disappear at 80% charge.
// This is 4.0v. Voltage divider on hardware / 2 so this is 60% of
// full scale.  LiPo min charge is 3.4v (51% scale). 
// Full scale is 1023.

#define BATTERY_MAX 620
#define BATTERY_MIN 527

#define BATTERY_MAX_LEVEL 5

namespace Battery
{

int32_t tick;
int8_t level;
bool draw;
bool depleted = false;

void init()
{
    tick = 0;
    level = 0;
    draw = true;

    pinMode(PIN_CHG, INPUT_PULLUP);
    pinMode(PIN_V_BAT, INPUT);
}

bool isDepleted()
{
    return depleted;
}

bool isCharging()
{
    return !digitalRead(PIN_CHG);
}

bool update()
{
    bool result = false;

    if (isCharging())
    {
        draw = true;
        depleted = false;

        if (millis() - tick > CHARGE_ANIMATION_FRAME_TICKS)
        {
            level = (level + 1) % (BATTERY_MAX_LEVEL + 1);
            tick = millis();
            result = true;
        }
    }
    else
    {
        int32_t reading = analogRead(PIN_V_BAT);
        int32_t range = BATTERY_MAX - BATTERY_MIN;

        reading -= BATTERY_MIN;

        depleted = (reading < 0);
        if (depleted)
        {
            reading = 0;
        }

        reading = (reading * ( BATTERY_MAX_LEVEL + 1)) / range;

        if (reading > BATTERY_MAX_LEVEL)
        {
            reading = BATTERY_MAX_LEVEL;
        }

        if (reading != level && millis() - tick > CHARGE_ANIMATION_FRAME_TICKS)
        {
            tick = millis();
            level = reading;
            result = true;
        }

        if (level == 0)
        {
            if (millis() - tick > CHARGE_ANIMATION_FRAME_TICKS)
            {
                draw = !draw;
                tick = millis();
                result = true;
            }
        }
        else
        {
            draw = true;
        }
    }

    return result;
}

void render()
{
    if (draw)
    {
        display.setFont(u8g2_font_battery19_tn);
        display.setFontDirection(1);
        display.setCursor(105, 0);
        display.print((char)(0x30 + level));
        display.setFontDirection(0);
    }
}
} // namespace Battery

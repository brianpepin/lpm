#include <globals.h>
#include <power.h>

#define POWER_OFF_TIMEOUT 1000

#define POWER_BUTTON_UNKNOWN 0
#define POWER_BUTTON_RELEASED 1
#define POWER_BUTTON_PRESSED 2

#define SLEEP_TIMEOUT (10 * 60)

namespace Power
{
uint8_t powerButtonState;
uint32_t pressedMillis;
uint32_t wakeTime;

void init()
{
    pinMode(PIN_PWR_CPU, OUTPUT);
    digitalWrite(PIN_PWR_CPU, HIGH);
    powerButtonState = POWER_BUTTON_UNKNOWN;
    wakeTime = millis() / 1000;
}

void resetSleep()
{
    wakeTime = millis() / 1000;
}

void tick()
{
    uint32_t now = millis() / 1000;
    uint32_t wakeDelta = now > wakeTime ? now - wakeTime : wakeTime - now;

    if (wakeDelta > SLEEP_TIMEOUT)
    {
        turnOff();
    }
    else if (powerButtonState == POWER_BUTTON_PRESSED)
    {
        uint32_t ellapsedMillis = millis();
        if (ellapsedMillis < pressedMillis)
        {
            // Timeout has wrapped counter.  Should be rare, so just re-acquire
            // millis.
            pressedMillis = ellapsedMillis;
        }
        else if (ellapsedMillis - pressedMillis >= POWER_OFF_TIMEOUT)
        {
            turnOff();
        }
    }
}

void processPowerButton(ButtonState buttonState)
{
    if (buttonState.changed)
    {
        digitalWrite(PIN_PWR_CPU, HIGH);
        powerButtonState = buttonState.pressed ? POWER_BUTTON_PRESSED : POWER_BUTTON_RELEASED;
        if (buttonState.pressed)
        {
            pressedMillis = millis();
        }
    }
}

void turnOff()
{
    powerButtonState = POWER_BUTTON_UNKNOWN;
    digitalWrite(PIN_PWR_CPU, LOW);
}
} // namespace Power

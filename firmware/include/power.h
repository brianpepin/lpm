
#pragma once

#include <button.h>

namespace Power
{
    void init();
    void processPowerButton(ButtonState buttonState);
    void tick();
    void resetSleep();
    void turnOff();
} // namespace Power
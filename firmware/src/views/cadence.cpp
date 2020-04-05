#include <globals.h>
#include <views/cadence.h>
#include <logger.h>
#include <settings.h>
#include <format.h>

#define DIGIT_WIDTH 15
#define MARGIN_TOP  35
#define MARGIN_LEFT 30
#define MARGIN_ARROW_TOP  33
#define MARGIN_ARROW_LEFT 20
#define MARGIN_TEXT_BOTTOM 50

#define CADENCE_MIN 1
#define CADENCE_MAX 60

void CadenceView::init(View *previousView)
{
    _previousView = previousView;
    _cadence = Settings::get()->logCadence;
}

bool CadenceView::processDpad(const DPad::Action &action, View **newView)
{
    bool update = false;

    if ((action.state.changed && action.state.pressed) || action.state.repeat)
    {
        switch(action.button)
        {
            case Button_Left:
                *newView = _previousView;
                update = true;
                break;

            case Button_Up:
                if (_cadence < CADENCE_MAX)
                {
                    _cadence++;
                    update = true;
                }
                break;

            case Button_Down:
                if (_cadence > CADENCE_MIN)
                {
                    _cadence--;
                    update = true;
                }
                break;

            case Button_Select:
                Settings::get()->logCadence = _cadence;
                Settings::save();
                *newView = _previousView;
                update = true;
                break;
        }
    }

    return update;
}

void CadenceView::render()
{
    display.setFont(MENU_HEADER_FONT);
    display.setCursor(12, display.getAscent());
    display.print(F("Log Cadence"));
    display.drawHLine(12, display.getAscent() + 2, display.getDisplayWidth() - 12);

    display.setFont(MENU_ARROW_FONT);
    display.setCursor(MARGIN_ARROW_LEFT, MARGIN_ARROW_TOP);
    display.print((char)77);

    display.setFont(NUMBER_SELECTOR_FONT);
    display.setCursor(MARGIN_LEFT, MARGIN_TOP);

    char text[8];
    Format::toInteger(_cadence, 2, text);
    display.print(text);

    display.drawHLine(MARGIN_LEFT, MARGIN_TOP + 1, 16);
    display.drawHLine(MARGIN_LEFT, MARGIN_TOP + 2, 16);

    display.setFont(MENU_ITEM_FONT);
    display.print(F(" sec"));

    // Now, how much logging time is this?
    char time[20];
    uint32_t logTime = (uint32_t)Logger::maxSamples() * _cadence;
    Format::toTime(logTime, time);

    display.setCursor(MARGIN_LEFT, MARGIN_TEXT_BOTTOM);
    display.print(time);
}

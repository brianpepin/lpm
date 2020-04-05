#include <globals.h>
#include <views/calibrate.h>
#include <adc.h>
#include <settings.h>

#define DIGIT_WIDTH 15
#define MARGIN_TOP  30
#define MARGIN_LEFT 20
#define MARGIN_ARROW_TOP  28
#define MARGIN_ARROW_LEFT 10
#define MARGIN_TEXT_TOP   50

#define REF_INSUFFICIENT() (_initialReading < Adc::maxValue / 2)

int32_t getValue(int8_t *digits)
{
    int32_t value = 0;
    for (int i = 0; i < 6; i++)
    {
        value = value * 10 + digits[i];
    }
    return value;
}

void CalibrateView::init(View *previousView)
{
    Adc::read(&_initialReading);
    int32_t value = Adc::toPower(_initialReading, 3);

    _initialCalibration = Adc::getCalibration();
    _select = 0;
    _previousView = previousView;

    for (int i = 0; i < 6; i++)
    {
        _digits[5-i] = value % 10;
        value /= 10;
    }
}

bool CalibrateView::processDpad(const DPad::Action &action, View **newView)
{
    if (_saveMillis != 0)
    {
        int32_t delta = millis() - _saveMillis;
        if (delta < 0 || delta > 1000)
        {
            _saveMillis = 0;
            *newView = _previousView;
            return true;
        }
    }

    bool update = false;

    if (action.state.changed && action.state.pressed)
    {
        if (REF_INSUFFICIENT())
        {
            _select = -1;
            update = true;
        }
        else
        {
            switch(action.button)
            {
                case Button_Right:
                    _select = (_select + 1) % 6;
                    update = true;
                    break;

                case Button_Left:
                    _select--;
                    update = true;
                    break;

                case Button_Up:
                    _digits[_select] = (_digits[_select] + 1) % 10;
                    update = true;
                    break;

                case Button_Down:
                    _digits[_select]--;
                    update = true;
                    break;

                case Button_Select:
                    Adc::setCalibration(_initialReading, getValue(_digits), true);
                    update = true;
                    _saveMillis = millis();
                    break;
            }
        }
    }

    if (update)
    {
        if (_digits[_select] < 0) _digits[_select] = 9;

        if (_select < 0)
        {
            _select = 0;
            Adc::setCalibration(_initialCalibration, false);
            *newView = _previousView;
        }
        else
        {
            int32_t newValue = getValue(_digits);
            Adc::setCalibration(_initialReading, newValue, false);
        }
    }

    return update;
}

bool CalibrateView::update(bool newReading)
{
    if (newReading)
    {
        int32_t reading;
        Adc::read(&reading);
        _currentReading = reading;
        return  true;
    }

    return false;
}

void CalibrateView::render()
{
    if (REF_INSUFFICIENT())
    {
        display.setCursor(MARGIN_LEFT, MARGIN_TOP);
        display.setFont(NOTICE_FONT);
        display.print(F("Insufficient reference"));
        display.setCursor(MARGIN_LEFT, MARGIN_TOP + 12);
        display.print(F("voltage for calibration"));
    }
    else if (_saveMillis)
    {
        display.setCursor(MARGIN_LEFT, MARGIN_TOP);
        display.setFont(NOTICE_FONT);
        display.print(F("Calibration saved"));
    }
    else
    {
        display.setFont(MENU_ARROW_FONT);
        display.setCursor(MARGIN_ARROW_LEFT, MARGIN_ARROW_TOP);
        display.print((char)77);

        display.setFont(NUMBER_SELECTOR_FONT);
        for (int i = 0; i < 6; i++)
        {
            display.setCursor(MARGIN_LEFT + i * DIGIT_WIDTH, MARGIN_TOP);
            display.print(_digits[i]);
            if (i == 2)
            {
                display.setCursor(MARGIN_LEFT + 8 + i * DIGIT_WIDTH, MARGIN_TOP);
                display.print(F("."));
            }
        }

        display.drawHLine(MARGIN_LEFT + _select * DIGIT_WIDTH, MARGIN_TOP + 1, 8);
        display.drawHLine(MARGIN_LEFT + _select * DIGIT_WIDTH, MARGIN_TOP + 2, 8);

        display.setCursor(MARGIN_LEFT, MARGIN_TEXT_TOP);
        display.setFont(MENU_ITEM_FONT);
        display.print(F("Reference: "));

        char value[40];
        Adc::toPower(_currentReading, 4, true, value);
        display.print(value);
    }
}

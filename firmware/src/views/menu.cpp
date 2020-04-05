#include <globals.h>
#include <views/menu.h>
#include <adc.h>
#include <power.h>
#include <logger.h>

#define MI_CLEAR        0
#define MI_LOGGING      1
#define MI_CALIBRATION  2
#define MI_POWER_OFF    3

const char PROGMEM _menuTitle[]     = "Menu";
const char PROGMEM _miClear[]       = "Clear";
const char PROGMEM _miLogging[]     = "Logging";
const char PROGMEM _miStopLogging[] = "Stop Logging";
const char PROGMEM _miCalibration[] = "Calibration";
const char PROGMEM _miPowerOff[]    = "Power Off";

const char * _menuText[] =
{
    _miClear,
    _miLogging,
    _miCalibration,
    _miPowerOff,
    nullptr
};

const char * _loggingMenuText[] =
{
    _miClear,
    _miStopLogging,
    _miCalibration,
    _miPowerOff,
    nullptr
};

void MenuView::init(View *previousView)
{
    MenuBaseView::init(previousView, _menuTitle);
}

bool MenuView::update(bool newReading)
{
    auto items = Logger::isLogging() ? _loggingMenuText : _menuText;
    if (getItems() != items)
    {
        setItems(items);
        return true;
    }
    return false;
}

void MenuView::onSelect(int8_t row, View **newView)
{
    switch (row)
    {
    case MI_CLEAR:
        Adc::zero();
        break;

    case MI_POWER_OFF:
        Power::turnOff();
        break;

    case MI_CALIBRATION:
        _calibrate.init(this);
        *newView = &_calibrate;
        break;

    case MI_LOGGING:
        if (Logger::isLogging())
        {
            Logger::stop();
            *newView = this;
        }
        else
        {
            _log.init(this);
            *newView = &_log;
        }
        break;
    }
}

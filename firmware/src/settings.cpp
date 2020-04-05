#include <globals.h>
#include <settings.h>
#include <Flash.h>

#define SETTINGS_VERSION 2

bool _initialized = false;

struct SettingsFlash
{
    uint8_t version;
    Settings settings;
} _settings;

Settings *Settings::get()
{
    if (!_initialized)
    {
        Flash::read(Flash::Addresses::settings, sizeof(_settings), &_settings);
        if (_settings.version != SETTINGS_VERSION)
        {
            switch(_settings.version)
            {
                case 1:
                    _settings.settings.logCadence = 0;
                    break;

                default:
                    _settings.settings = { };
                    break;
            }
            _settings.version = SETTINGS_VERSION;
        }

        _initialized = true;
    }

    return &_settings.settings;
}

void Settings::save()
{
    if (!_initialized)
    {
        get();
    }

    Flash::write(Flash::Addresses::settings, sizeof(_settings), &_settings);
}

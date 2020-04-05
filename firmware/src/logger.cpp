#include <globals.h>
#include <logger.h>
#include <flash.h>
#include <settings.h>
#include <format.h>
#include <graphics.h>

#define LOGGER_VERSION 1
#define INVALID_TIMESTAMP 0xFFFFFFFF

#define DEFAULT_LOG_CADENCE 5

#define BLINK_CADENCE 500

namespace Logger
{
    struct Header
    {
        uint32_t version;
    };

    struct Entry
    {
        uint32_t timestamp;
        int32_t reading;
    };

    constexpr static Entry invalid = {INVALID_TIMESTAMP, 0};

    uint32_t _cadenceMillis = 0;
    uint32_t _lastLogMillis = 0;
    Entry _currentEntry = { };
    uint16_t _currentIndex = 0;
    uint32_t _blinkMillis = 0;
    bool _isBlink = false;

    bool validateHeader()
    {
        Header header;
        Flash::read(Flash::Addresses::logger, sizeof(header), &header);
        return (header.version == LOGGER_VERSION);
    }

    void writeHeader()
    {
        Header header;
        header.version = LOGGER_VERSION;
        Flash::write(Flash::Addresses::logger, sizeof(header), &header);
    }

    bool indexToAddress(uint16_t index, uint16_t* address)
    {
        uint32_t a = index * sizeof(Entry) + Flash::Addresses::logger + sizeof(Header);
        if (a < Flash::Addresses::max)
        {
            *address = (uint16_t)a;
            return true;
        }
        return false;
    }

    bool readEntry(uint16_t index, Entry* entry)
    {
        uint16_t address;
        if (indexToAddress(index, &address))
        {
            Flash::read(address, sizeof(Entry), entry);
            return true;
        }
        return false;
    }

    bool writeEntry(uint16_t index, const Entry& entry)
    {
        uint16_t address;
        if (indexToAddress(index, &address))
        {
            Flash::write(address, sizeof(Entry), &entry);
            return true;
        }
        return false;
    }

    void init()
    {
        if (Settings::get()->logCadence == 0)
        {
            Settings::get()->logCadence = DEFAULT_LOG_CADENCE;
        }
    }

    void start(uint8_t cadenceSeconds)
    {
        clear();

        if (!validateHeader())
        {
            writeHeader();
        }

        _cadenceMillis = cadenceSeconds;
        _cadenceMillis *= 1000;

        _lastLogMillis = millis();
        _blinkMillis = _lastLogMillis;
        _currentIndex = 0;
        _currentEntry.timestamp = 0;
        _currentEntry.reading = 0;
    }

    void stop()
    {
        _cadenceMillis = 0;
        _blinkMillis = 0;
        _isBlink = false;
    }

    void clear()
    {
        stop();
        writeEntry(0, invalid);
    }

    bool isLogging()
    {
        return _cadenceMillis != 0;
    }

    bool isEmpty()
    {
        if (!validateHeader())
        {
            return true;
        }

        Entry entry;
        readEntry(0, &entry);
        return entry.timestamp == INVALID_TIMESTAMP;
    }

    uint16_t maxSamples()
    {
        return (Flash::Addresses::max - Flash::Addresses::logger - sizeof(Header)) /
            sizeof(Entry) - 1;
    }

    bool fetch(uint16_t index, uint32_t* timestamp, int32_t* reading)
    {
        if (!validateHeader())
        {
            return false;
        }

        Entry entry;
        if (!readEntry(index, &entry))
        {
            return false;
        }

        if (entry.timestamp == INVALID_TIMESTAMP)
        {
            return false;
        }

        *timestamp = entry.timestamp;
        *reading = entry.reading;

        return true;
    }

    bool update(int32_t reading)
    {
        bool updated = false;

        if (_cadenceMillis != 0)
        {
            uint32_t currentMillis = millis();
            uint32_t delta;

            if (currentMillis >= _lastLogMillis)
            {
                delta = currentMillis - _lastLogMillis;
            }
            else
            {
                delta = 0xFFFFFFFF - _lastLogMillis + currentMillis;
            }

            if (delta >= _cadenceMillis)
            {
                _currentEntry.timestamp += (_cadenceMillis / 1000);

                if (writeEntry(_currentIndex, _currentEntry) &&
                    writeEntry(_currentIndex + 1, invalid))
                {
                    _currentIndex++;
                    _currentEntry.reading = 0;
                    _lastLogMillis = currentMillis;
                }
                else
                {
                    writeEntry(_currentIndex, invalid);
                    stop();
                }
                updated = true;
            }
            else
            {
                _currentEntry.reading -= _currentEntry.reading / 2;
                _currentEntry.reading += reading / 2;
            }

            if (currentMillis < _blinkMillis || (currentMillis - _blinkMillis) > BLINK_CADENCE)
            {
                _isBlink = !_isBlink;
                _blinkMillis = currentMillis;
                updated = true;
            }
        }

        return updated;
    }

    void render()
    {
        if (isLogging())
        {
            char str[20];
            uint32_t seconds = (uint32_t)(maxSamples() - _currentIndex) * (_cadenceMillis / 1000);
            Format::toTime(seconds, str);
            display.setFont(LOGGING_INDICATOR_FONT);
            display.setCursor(92 - display.getStrWidth(str), 7);
            display.print(str);

            if (_isBlink) display.drawDisc(98, 4, 2);
        }
    }
}

#include <globals.h>
#include <adc.h>
#include <settings.h>
#include <format.h>
#include <imath.h>

// 24 bit ADC measuring our 12 V value means
// a minimum quanta of 1.4uV.  We need to multiply
// by at least 1/quanta to allow integer divides
// to not truncate. This is the number of zeros to
// use.

#define ADC_RESOLUTION 7

namespace Adc
{
int32_t _reading;
int32_t _average;
int32_t _max;

void clock(int pin)
{
    digitalWrite(pin, HIGH);
    delayMicroseconds(1);
    digitalWrite(pin, LOW);
    delayMicroseconds(1);
}

void init()
{
    // Initialize A/D converter
    pinMode(PIN_ADC_DRDY, INPUT);
    pinMode(PIN_ADC_SCK, OUTPUT);
    pinMode(PIN_ADC_PWR, OUTPUT);
    pinMode(PIN_ADC_TEMP, OUTPUT);

    digitalWrite(PIN_ADC_SCK, LOW);
    digitalWrite(PIN_ADC_TEMP, LOW);
    digitalWrite(PIN_ADC_PWR, HIGH);

    // Calibrate by reading one reading
    // and clocking two past it
    while (digitalRead(PIN_ADC_DRDY))
    {
        delayMicroseconds(10);
    }

    for (uint8_t ck = 0; ck < 26; ck++)
    {
        clock(PIN_ADC_SCK);
    }

    if (Settings::get()->calibrationSlope == 0)
    {
        setCalibration(Adc::maxValue, 12000);
    }
}

void zero()
{
    _reading = _average = _max = 0;
}

bool update()
{
    bool dataReady = !digitalRead(PIN_ADC_DRDY);

    if (dataReady)
    {
        int32_t adc = 0;

        for (uint8_t idx = 0; idx < 3; idx++)
        {
            adc |= shiftIn(PIN_ADC_DRDY, PIN_ADC_SCK, MSBFIRST);
            adc = adc << 8;
        }

        adc = adc >> 8;

        // One extra clock pulse to get drdy waiting again
        clock(PIN_ADC_SCK);

        _reading = adc;
        _average -= _average / 10;
        _average += adc / 10;

        if (adc > _max)
        {
            _max = adc;
        }
    }

    return dataReady;
}

void read(int32_t *reading, int32_t *average, int32_t *max)
{
    *reading = _reading;
    if (average) *average = _average;
    if (max) *max = _max;
}

int32_t toPower(int32_t reading, uint8_t precision)
{
    int64_t value = reading;
    value *= Settings::get()->calibrationSlope;
    value /= 1000;

    if (precision < ADC_RESOLUTION)
    {
        value /= ipow64(10, ADC_RESOLUTION - precision);
    }

    return (int32_t)value;
}

void toPower(int32_t reading, uint8_t precision, bool units, char * str)
{
    int64_t value = toPower(reading, precision);
    bool mw = false;

    if (units && value < ipow64(10, precision))
    {
        mw = true;
        value *= 1000;
    }

    if (value < 0)
    {
        value = 0;
    }

    size_t cch = Format::toDecimal(value, precision, str);
    char *pch = str + cch;

    if (units)
    {
        if (mw)
        {
            *(pch++) = 'm';
        }

        *(pch++) = 'w';
        *pch = '\0';
    }
}

void setCalibration(int32_t reading, int32_t power, bool save)
{
    setCalibration((power * ipow64(10, ADC_RESOLUTION)) / reading, save);
}

void setCalibration(int32_t value, bool save)
{
    Settings::get()->calibrationSlope = value;

    if (save)
    {
        Settings::save();
    }
}

int32_t getCalibration()
{
    return Settings::get()->calibrationSlope;
}
} // namespace Adc

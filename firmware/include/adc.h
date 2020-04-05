
#pragma once

namespace Adc
{
    constexpr int32_t maxValue = 0x7FFFFF;

    void init();
    void zero();
    bool update();
    void read(int32_t* reading, int32_t* average = nullptr, int32_t* max = nullptr);

    int32_t toPower(int32_t reading, uint8_t precision);
    void toPower(int32_t reading, uint8_t precision, bool units, char* str);

    int32_t getCalibration();
    void setCalibration(int32_t value, bool save);
    void setCalibration(int32_t reading, int32_t power, bool save);
} // namespace Adc

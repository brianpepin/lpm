#pragma once

namespace Format
{
    size_t toDecimal(int32_t value, uint8_t precision, char* str);
    size_t toInteger(int32_t value, uint8_t minDigits, char* str);
    size_t toTime(uint32_t seconds, char* str);
}
#include <globals.h>
#include <imath.h>

int64_t ipow64(int64_t x, int8_t y)
{
    int64_t value = 1;
    for (int8_t i = 0; i < y; i++)
    {
        value *= x;
    }
    return value;
}

int32_t ipow32(int32_t x, int8_t y)
{
    int32_t value = 1;
    for (int8_t i = 0; i < y; i++)
    {
        value *= x;
    }
    return value;
}
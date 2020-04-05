#include <globals.h>
#include <format.h>
#include <imath.h>

namespace Format
{
    size_t toDecimal(int32_t value, uint8_t precision, char* str)
    {
        char *pch = str;
        bool negate = value < 0;

        if (negate) value *= -1;

        do
        {
            *pch++ = (char)((value % 10) + 0x30);
            value /= 10;

            if (pch - str == precision) *pch++ = '.';
        } while (value != 0 || (precision != 0 && pch - str <= precision + 1));

        if (negate) *(pch++) = '-';

        // String needs to be reversed
        unsigned int i = 0;
        unsigned int j = pch-str - 1;

        while (i < j)
        {
            char t = str[i];
            str[i] = str[j];
            str[j] = t;
            i++;
            j--;
        }
        
        // Drop trailing zeros
        if (precision)
        {
            while (--pch != str && (*pch == '0' && *(pch - 1) != '.'));

            pch++;
        }
        *pch = '\0';

        return pch - str;
    }

    size_t toInteger(int32_t value, uint8_t minDigits, char* str)
    {
        char* pch = str;
        uint32_t pow = ipow32(10, minDigits) / 10;
        while(pow != 0)
        {
            if (value / pow == 0) *pch++ = '0';
            pow /= 10;
        }

        if (value)
        {
            size_t cch = toDecimal(value, 0, pch);
            return pch - str + cch;
        }
        else
        {
            *pch = '\0';
            return pch - str;
        }
    }

    size_t toTime(uint32_t seconds, char* str)
    {
        uint32_t days = seconds / 86400; seconds -= (days * 86400);
        uint32_t hours = seconds / 3600; seconds -= (hours * 3600);
        uint32_t minutes = seconds / 60; seconds -= (minutes * 60);

        char* pch = str;
        size_t cch;

        if (days)
        {
            cch = toInteger((int32_t)days, 2, pch);
            pch += cch;
            *pch++ = 'd';
            *pch++ = ' ';
        }

        if (days || hours)
        {
            cch = toInteger((int32_t)hours, 2, pch);
            pch += cch;
            *pch++ = ':';
        }

        if (days || hours || minutes)
        {
            cch = toInteger((int32_t)minutes, 2, pch);
            pch += cch;
            *pch++ = ':';
        }

        cch = toInteger((int32_t)seconds, 2, pch);
        pch += cch;
        *pch++ = 's';
        *pch = '\0';

        return pch - str;
    }
}
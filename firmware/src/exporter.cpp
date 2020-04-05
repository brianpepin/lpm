#include <globals.h>
#include <exporter.h>
#include <logger.h>
#include <adc.h>

namespace Exporter
{
    void exportLog()
    {
        uint16_t index = 0;
        uint32_t timestamp;
        int32_t reading;
        char power[20];

        while(Logger::fetch(index++, &timestamp, &reading))
        {
            Adc::toPower(reading, 4, false, power);
            Serial.print(timestamp);
            Serial.print(F(", "));
            Serial.println(power);
        }
    }
}
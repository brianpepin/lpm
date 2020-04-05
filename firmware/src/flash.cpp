#include <globals.h>
#include <flash.h>

// Opcodes
#define WREN  0x06 /* Write enable */
#define WRDI  0x04 /* Write disable */
#define RDSR  0x05 /* Read status reg */
#define WRSR  0x01 /* Write status reg */
#define READ  0x03 /* Read data */
#define WRITE 0x02 /* Write data */

// Status reg bits
#define WPEN 0b10000000
#define IPL  0b01000000
#define LIP  0b00010000
#define BP1  0b00001000
#define BP0  0b00000100
#define WEL  0b00000010
#define RDY  0b00000001

#define PAGE_SIZE 32

namespace Flash
{
    SPISettings _settings(10000000, MSBFIRST, SPI_MODE0);

    void waitReady()
    {
        uint8_t status;
        do
        {
            digitalWrite(PIN_FLASH_CS, LOW);
            SPI.transfer(RDSR);
            status = SPI.transfer(0);
            digitalWrite(PIN_FLASH_CS, HIGH);
        } while (status & RDY);
    }

    void init()
    {
        pinMode(PIN_FLASH_CS, OUTPUT);
        digitalWrite(PIN_FLASH_CS, HIGH);
        SPI.begin();
    }

    void read(uint16_t address, size_t cb, void* buffer)
    {
        SPI.beginTransaction(_settings);

        waitReady();

        digitalWrite(PIN_FLASH_CS, LOW);

        SPI.transfer(READ);
        SPI.transfer16(address);

        uint8_t *pb = (uint8_t*)(buffer);
        for (size_t b = 0; b < cb; b++)
        {
            // Data reads out just by running
            // the clock; the transfer in is
            // ignored.
            pb[b] = SPI.transfer(0);
        }

        digitalWrite(PIN_FLASH_CS, HIGH);
        SPI.endTransaction();
    }

    void write(uint16_t address, size_t cb, const void* buffer)
    {
        SPI.beginTransaction(_settings);

        uint8_t *pb = (uint8_t*)(buffer);
        for (size_t b = 0; b < cb; b++)
        {
            if (b == 0 || address % PAGE_SIZE == 0)
            {
                digitalWrite(PIN_FLASH_CS, HIGH);

                waitReady();

                digitalWrite(PIN_FLASH_CS, LOW);
                SPI.transfer(WREN);
                digitalWrite(PIN_FLASH_CS, HIGH);

                // Needs to be min 80ns between low to high
                // transitions, but digitalWrite is slower
                // than that.  If we were doing direct port
                // IO we may need a delay.

                digitalWrite(PIN_FLASH_CS, LOW);

                SPI.transfer(WRITE);
                SPI.transfer16(address);
            }

            SPI.transfer(pb[b]);
            address++;
        }

        digitalWrite(PIN_FLASH_CS, HIGH);
        SPI.endTransaction();
    }
}
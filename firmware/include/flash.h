#pragma once

namespace Flash
{
    namespace Addresses
    {
        constexpr uint16_t settings = 0x00;
        constexpr uint16_t logger = 0x20;
        constexpr uint32_t max = 0x10000;
    }

    void init();
    void read(uint16_t address, size_t cb, void* buffer);
    void write(uint16_t address, size_t cb, const void* buffer);
}
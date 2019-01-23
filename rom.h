#pragma once

#include<cstdint>


struct Rom
{
    // PRG-ROM
    uint8_t*    prg;
    // CHR-ROM
    uint8_t*    chr;
    // 16KB PRG size counter
    uint32_t    count_16k;
    // 8KB CHR size counter
    uint32_t    count_8k;
    // Mapper number
    uint8_t     mapper_number;
    // Vertical Mirroring
    uint8_t     vmirroring;
    // Four-screen mode
    uint8_t     four_screen;
    // Save ram
    uint8_t     save_ram;
    // Reserved
    uint8_t     reserved[4];
};

struct NesHeader{
    uint32_t    id;
    uint8_t     count_16k;
    uint8_t     count_8k;
    uint8_t     flag6;
    uint8_t     flag7;
    uint8_t     reserved[8];
};


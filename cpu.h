#pragma once

#include "famicom.h"
#include <cstdint>


enum cpuVector{
    NMI     = 0xFFFA,
    RESET   = 0xFFFC,
    IRQBRK  = 0xFFFE
};

class Cpu
{
private:
    Famicom* famicom;
public:
    Cpu(Famicom&);
    uint8_t read(uint16_t);
    void write(uint16_t, uint8_t);
};

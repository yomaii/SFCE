#include "famicom.h"
#include "cpu.h"
#include <iostream>

int main() {
    Famicom famicom;
    famicom.init("/Users/lolly/prog/sfce/nestest.nes");
    famicom.showInfo();
    Cpu cpu(famicom);
    // V0 - NMI
    uint16_t v0 = cpu.read(NMI + 0);
    v0 |= cpu.read(NMI + 1) << 8;
    // V1 - RESET
    uint16_t v1 = cpu.read(RESET + 0);
    v1 |= cpu.read(RESET + 1) << 8;
    // V2 - IRQ/BRK
    uint16_t v2 = cpu.read(IRQBRK + 0);
    v2 |= cpu.read(IRQBRK + 1) << 8;

    printf(
        "ROM: NMI: $%04X  RESET: $%04X  IRQ/BRK: $%04X\n",
        (int)v0, (int)v1, (int)v2
    );
    return 0;
}
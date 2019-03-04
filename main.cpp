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
    char b0[DISASSEMBLY_BUF_LEN2], b1[DISASSEMBLY_BUF_LEN2], b2[DISASSEMBLY_BUF_LEN2];
    cpu.disassemblyPos(v0, b0);
    cpu.disassemblyPos(v1, b1);
    cpu.disassemblyPos(v2, b2);
    printf(
        "NMI:     %s\n"
        "RESET:   %s\n"
        "IRQ/BRK: %s\n",
        b0, b1, b2
    );
    while(1){
        cpu.executeOne();
    }
    return 0;
}
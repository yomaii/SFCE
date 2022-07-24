#include "famicom.h"
#include <iostream>
#include <string>
#include "2d.h"
Famicom famicom;

int main() {
    
    famicom.Init("/Users/lolly/prog/SFCE/smb.nes");
    famicom.ShowInfo();

    // show cpu vectors
    // V0 - NMI
    uint16_t v0 = famicom.cpu_->Read(CPU_NMI + 0);
    v0 |= famicom.cpu_->Read(CPU_NMI + 1) << 8;
    // V1 - RESET
    uint16_t v1 = famicom.cpu_->Read(CPU_RESET + 0);
    v1 |= famicom.cpu_->Read(CPU_RESET + 1) << 8;
    // V2 - IRQ/BRK
    uint16_t v2 = famicom.cpu_->Read(CPU_IRQBRK + 0);
    v2 |= famicom.cpu_->Read(CPU_IRQBRK + 1) << 8;

    printf(
        "ROM: NMI: $%04X  RESET: $%04X  IRQ/BRK: $%04X\n",
        (int)v0, (int)v1, (int)v2
    );
    CreateWindow();
    return 0;
}
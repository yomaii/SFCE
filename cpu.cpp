#include "cpu.h"
#include <iostream>


Cpu::Cpu(Famicom& fa){
    famicom = &fa;
}
uint8_t Cpu::read(uint16_t address){
    /*
    +---------+-------+-------+-----------------------+
    | Address | Size  | Flags | Description           |
    +---------+-------+-------+-----------------------+
    | $0000   | $800  |       | RAM                   |
    | $0800   | $800  | M     | RAM                   |
    | $1000   | $800  | M     | RAM                   |
    | $1800   | $800  | M     | RAM                   |
    | $2000   | 8     |       | Registers             |
    | $2008   | $1FF8 |  R    | Registers             |
    | $4000   | $20   |       | Registers             |
    | $4020   | $1FE0 |       | Expansion I/O         |
    | $6000   | $2000 |       | SRAM                  |
    | $8000   | $4000 |       | PRG-ROM               |
    | $C000   | $4000 |       | PRG-ROM               |
    +---------+-------+-------+-----------------------+
    */
    switch(address >> 13){
    case 0:
        // [$0000,$2000) RAM
        return famicom->mainMemory[address & (uint16_t)0x07ff];
    case 1:
        // [$2000,$4000) not suported yet
        assert(!"NOT IMPL");
        return 0;
    case 2:
        // [$4000,$6000) not suported yet
        assert(!"NOT IMPL");
        return 0;
    case 3:
        // [$6000,$8000) SRAM
        return famicom->saveMemory[address & (uint16_t)0x1fff];
    case 4: case 5: case 6: case 7:
        // [$8000,$10000) PRG-ROM
        return famicom->prgBanks[address >> 13][address & (uint16_t)0x1fff];
    default:
        assert(!"invalid address");
    }
    return 0;

}
void Cpu::write(uint16_t address, uint8_t data){
    switch(address >> 13){
    case 0:
        // [$0000,$2000) RAM
        famicom->mainMemory[address & (uint16_t)0x07ff] = data;
        return;
    case 1:
        // [$2000,$4000) not suported yet
        assert(!"NOT IMPL");
        return;
    case 2:
        // [$4000,$6000) not suported yet
        assert(!"NOT IMPL");
        return;
    case 3:
        // [$6000,$8000) SRAM
        famicom->saveMemory[address & (uint16_t)0x1fff] = data;
        return;
    case 4: case 5: case 6: case 7:
        // [$8000,$10000) PRG-ROM
        famicom->prgBanks[address >> 13][address & (uint16_t)0x1fff] = data;
        return;
    default:
        assert(!"invalid address");
    }
}
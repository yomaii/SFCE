#include "cpu.h"

#include <iostream>

Cpu::Cpu(){}
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
        assert(!"WARNING: PRG-ROM");
        famicom->prgBanks[address >> 13][address & (uint16_t)0x1fff] = data;
        return;
    default:
        assert(!"invalid address");
    }
}

void Cpu::disassemblyPos(uint16_t address, char* buf){
    enum {
        OFFSET_M = DISASSEMBLY_BUF_LEN2 - DISASSEMBLY_BUF_LEN,
        OFFSET = 8
    };
    static_assert(OFFSET < OFFSET_M, "LESS!");
    memset(buf, ' ', OFFSET);
    buf[0] = '$';
    btoh(buf + 1, (uint8_t)(address >> 8));
    btoh(buf + 3, (uint8_t)(address));


    Code6502 code;
    code.data = 0;

    code.op = read(address);
    code.a1 = read(address + 1);
    code.a2 = read(address + 2);

    disassembly(code, buf + OFFSET);
}
void Cpu::btoh(char o[], uint8_t b) {
    o[0] = HEXDATA[b >> 4];
    o[1] = HEXDATA[b & (uint8_t)0x0F];
}

void Cpu::btod(char o[], uint8_t b) {
    const int8_t sb = (int8_t)b;
    if (sb < 0) {
        o[0] = '-';
        b = -b;
    }
    else o[0] = '+';
    o[1] = HEXDATA[(uint8_t)b / 100];
    o[2] = HEXDATA[(uint8_t)b / 10 % 10];
    o[3] = HEXDATA[(uint8_t)b % 10];
}

#define fallthrough
void Cpu::disassembly(Code6502 code, char* buf){
    enum {
        NAME_FIRSH = 0,
        ADDR_FIRSH = NAME_FIRSH + 4,
        LEN = ADDR_FIRSH + 9
    };
    memset(buf, ' ', LEN); buf[LEN] = ';'; buf[LEN + 1] = 0;
    static_assert(LEN + 1 < DISASSEMBLY_BUF_LEN, "");
    const struct OpName opname = OPNAMEDATA[code.op];
    buf[NAME_FIRSH + 0] = opname.name[0];
    buf[NAME_FIRSH + 1] = opname.name[1];
    buf[NAME_FIRSH + 2] = opname.name[2];
    switch (opname.mode)
    {
    case AM_UNK:
        fallthrough;
    case AM_IMP:
        // XXX     ;
        break;
    case AM_ACC:
        // XXX A   ;
        buf[ADDR_FIRSH + 0] = 'A';
        break;
    case AM_IMM:
        // XXX #$AB
        buf[ADDR_FIRSH + 0] = '#';
        buf[ADDR_FIRSH + 1] = '$';
        btoh(buf + ADDR_FIRSH + 2, code.a1);
        break;
    case AM_ABS:
        // XXX $ABCD
        fallthrough;
    case AM_ABX:
        // XXX $ABCD, X
        fallthrough;
    case AM_ABY:
        // XXX $ABCD, Y
        // REAL
        buf[ADDR_FIRSH] = '$';
        btoh(buf + ADDR_FIRSH + 1, code.a2);
        btoh(buf + ADDR_FIRSH + 3, code.a1);
        if (opname.mode == AM_ABS) break;
        buf[ADDR_FIRSH + 5] = ',';
        buf[ADDR_FIRSH + 7] = opname.mode == AM_ABX ? 'X' : 'Y';
        break;
    case AM_ZPG:
        // XXX $AB
        fallthrough;
    case AM_ZPX:
        // XXX $AB, X
        fallthrough;
    case AM_ZPY:
        // XXX $AB, Y
        // REAL
        buf[ADDR_FIRSH] = '$';
        btoh(buf + ADDR_FIRSH + 1, code.a1);
        if (opname.mode == AM_ZPG) break;
        buf[ADDR_FIRSH + 3] = ',';
        buf[ADDR_FIRSH + 5] = opname.mode == AM_ABX ? 'X' : 'Y';
        break;
    case AM_INX:
        // XXX ($AB, X)
        buf[ADDR_FIRSH + 0] = '(';
        buf[ADDR_FIRSH + 1] = '$';
        btoh(buf + ADDR_FIRSH + 2, code.a1);
        buf[ADDR_FIRSH + 4] = ',';
        buf[ADDR_FIRSH + 6] = 'X';
        buf[ADDR_FIRSH + 7] = ')';
        break;
    case AM_INY:
        // XXX ($AB), Y
        buf[ADDR_FIRSH + 0] = '(';
        buf[ADDR_FIRSH + 1] = '$';
        btoh(buf + ADDR_FIRSH + 2, code.a1);
        buf[ADDR_FIRSH + 4] = ')';
        buf[ADDR_FIRSH + 5] = ',';
        buf[ADDR_FIRSH + 7] = 'Y';
        break;
    case AM_IND:
        // XXX ($ABCD)
        buf[ADDR_FIRSH + 0] = '(';
        buf[ADDR_FIRSH + 1] = '$';
        btoh(buf + ADDR_FIRSH + 2, code.a2);
        btoh(buf + ADDR_FIRSH + 4, code.a1);
        buf[ADDR_FIRSH + 6] = ')';
        break;
    case AM_REL:
        // XXX $AB(-085)
        // XXX $ABCD
        buf[ADDR_FIRSH + 0] = '$';
        //const uint16_t target = base + int8_t(data.a1);
        //btoh(buf + ADDR_FIRSH + 1, uint8_t(target >> 8));
        //btoh(buf + ADDR_FIRSH + 3, uint8_t(target & 0xFF));
        btoh(buf + ADDR_FIRSH + 1, code.a1);
        buf[ADDR_FIRSH + 3] = '(';
        btod(buf + ADDR_FIRSH + 4, code.a1);
        buf[ADDR_FIRSH + 8] = ')';
        break;
    }
}

// 宏定义

// registers


#define OP(n, a, o) \
case 0x##n:\
{           \
    const uint16_t address = addressing.a();\
    printf("address: %X\n", address);\
    operation.o(address);\
    break;\
}

void Cpu::log(){
    static int line = 0;
    line++;
    char buf[DISASSEMBLY_BUF_LEN2];
    const uint16_t pc = famicom->registers.programCounter;
    disassemblyPos(pc, buf);
    printf(
        "%4d - %s   A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
        line, buf,
        (int)famicom->registers.accumulator,
        (int)famicom->registers.xIndex,
        (int)famicom->registers.yIndex,
        (int)famicom->registers.status,
        (int)famicom->registers.stackPointer
    );
}



void Cpu::executeOne(){
    log();
    const uint8_t opcode = read(REG_PC++);
    Addressing addressing(famicom);
    Operation operation(famicom);
    switch(opcode){
        OP(4C, ABS, JMP)
        OP(A2, IMM, LDX)
        OP(86, ZPG, STX)
        OP(20, ABS, JSR)
        OP(EA, IMP, NOP)
        OP(38, IMP, SEC)
        OP(B0, REL, BSC)
        OP(18, IMP, CLC)
        OP(90, REL, BCC)
        OP(A9, IMM, LDA)
        OP(F0, REL, BEQ)
        OP(D0, REL, BNE)
        OP(85, ZPG, STA)
        OP(24, ZPG, BIT)
        OP(70, REL, BVS)
        OP(50, REL, BVC)
        OP(10, REL, BPL)
        OP(60, IMP, RTS)
        OP(78, IMP, SEI)
        OP(F8, IMP, SED)
        OP(08, IMP, PHP)
        OP(68, IMP, PLA)
        OP(29, IMM, AND)
        OP(C9, IMM, CMP)
        OP(D8, IMP, CLD)
        OP(48, IMP, PHA)
        OP(28, IMP, PLP)
        OP(30, REL, BMI)
        OP(09, IMM, ORA)
        OP(B8, IMP, CLV)
        OP(49, IMM, EOR)
        OP(69, IMM, ADC)
        OP(A0, IMM, LDY)
        OP(C0, IMM, CPY)
        OP(E0, IMM, CPX)
    default:
        printf("%X\n", opcode);
        assert(!"exit!");
    }
    
}


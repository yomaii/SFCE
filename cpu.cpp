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
    printf("address: %X OP: %X\n", address, opcode);\
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
        OP(1A, IMP, NOP)
        OP(3A, IMP, NOP)
        OP(5A, IMP, NOP)
        OP(7A, IMP, NOP)
        OP(DA, IMP, NOP)
        OP(FA, IMP, NOP)
        OP(60, IMP, RTS)
        OP(78, IMP, SEI)
        OP(F8, IMP, SED)
        OP(08, IMP, PHP)
        OP(68, IMP, PLA)
        OP(80, IMM, NOP)
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
        OP(E9, IMM, SBC)
        OP(C8, IMP, INY)
        OP(E8, IMP, INX)
        OP(88, IMP, DEY)
        OP(CA, IMP, DEX)
        OP(A8, IMP, TAY)
        OP(AA, IMP, TAX)
        OP(98, IMP, TYA)
        OP(8A, IMP, TXA)
        OP(BA, IMP, TSX)
        OP(8E, ABS, STX)
        OP(9A, IMP, TXS)
        OP(AE, ABS, LDX)
        OP(AD, ABS, LDA)
        OP(40, IMP, RTI)
        OP(4A, ACC, LSRA)
        OP(0A, ACC, ASLA)
        OP(6A, ACC, RORA)
        OP(2A, ACC, ROLA)
        OP(A5, ZPG, LDA)
        OP(8D, ABS, STA)
        OP(A1, INX, LDA)
        OP(A3, INX, LAX)
        OP(81, INX, STA)
        OP(01, INX, ORA)
        OP(21, INX, AND)
        OP(41, INX, EOR)
        OP(61, INX, ADC)
        OP(C1, INX, CMP)
        OP(E1, INX, SBC)
        OP(04, ZPG, NOP)
        OP(44, ZPG, NOP)
        OP(64, ZPG, NOP)
        OP(A4, ZPG, LDY)
        OP(84, ZPG, STY)
        OP(A6, ZPG, LDX)
        OP(A7, ZPG, LAX)
        OP(05, ZPG, ORA)
        OP(25, ZPG, AND)
        OP(45, ZPG, EOR)
        OP(65, ZPG, ADC)
        OP(C5, ZPG, CMP)
        OP(E5, ZPG, SBC)
        OP(E4, ZPG, CPX)
        OP(C4, ZPG, CPY)
        OP(46, ZPG, LSR)
        OP(06, ZPG, ASL)
        OP(66, ZPG, ROR)
        OP(26, ZPG, ROL)
        OP(E6, ZPG, INC)
        OP(C6, ZPG, DEC)
        OP(0C, ABS, NOP)
        OP(AC, ABS, LDY)
        OP(8C, ABS, STY)
        OP(2C, ABS, BIT)
        OP(0D, ABS, ORA)
        OP(2D, ABS, AND)
        OP(4D, ABS, EOR)
        OP(6D, ABS, ADC)
        OP(CD, ABS, CMP)
        OP(ED, ABS, SBC)
        OP(EC, ABS, CPX)
        OP(CC, ABS, CPY)
        OP(4E, ABS, LSR)
        OP(0E, ABS, ASL)
        OP(6E, ABS, ROR)
        OP(2E, ABS, ROL)
        OP(EE, ABS, INC)
        OP(CE, ABS, DEC)
        OP(AF, ABS, LAX)
        OP(B1, INY, LDA)
        OP(11, INY, ORA)
        OP(31, INY, AND)
        OP(51, INY, EOR)
        OP(71, INY, ADC)
        OP(D1, INY, CMP)
        OP(F1, INY, SBC)
        OP(91, INY, STA)
        OP(B3, INY, LAX)
        OP(6C, IND, JMP)
        OP(14, ZPX, NOP)
        OP(15, ZPX, ORA)
        OP(16, ZPX, ASL)
        OP(34, ZPX, NOP)
        OP(35, ZPX, AND)
        OP(36, ZPX, ROL)
        OP(54, ZPX, NOP)
        OP(55, ZPX, EOR)
        OP(56, ZPX, LSR)
        OP(74, ZPX, NOP)
        OP(75, ZPX, ADC)
        OP(76, ZPX, ROR)
        OP(94, ZPX, STY)
        OP(95, ZPX, STA)
        OP(B4, ZPX, LDY)
        OP(B5, ZPX, LDA)
        OP(D4, ZPX, NOP)
        OP(D5, ZPX, CMP)
        OP(D6, ZPX, DEC)
        OP(F4, ZPX, NOP)
        OP(F5, ZPX, SBC)
        OP(F6, ZPX, INC)
        OP(B6, ZPY, LDX)
        OP(96, ZPY, STX)
        OP(1C, ABX, NOP)
        OP(1D, ABX, ORA)
        OP(1E, ABX, ASL)
        OP(3C, ABX, NOP)
        OP(3D, ABX, AND)
        OP(3E, ABX, ROL)
        OP(5C, ABX, NOP)
        OP(5D, ABX, EOR)
        OP(5E, ABX, LSR)
        OP(7C, ABX, NOP)
        OP(7D, ABX, ADC)
        OP(7E, ABX, ROR)
        OP(9D, ABX, STA)
        OP(BC, ABX, LDY)
        OP(BD, ABX, LDA)
        OP(DC, ABX, NOP)
        OP(DD, ABX, CMP)
        OP(DE, ABX, DEC)
        OP(FC, ABX, NOP)
        OP(FD, ABX, SBC)
        OP(FE, ABX, INC)
        OP(19, ABY, ORA)
        OP(39, ABY, AND)
        OP(59, ABY, EOR)
        OP(79, ABY, ADC)
        OP(99, ABY, STA)
        OP(B9, ABY, LDA)
        OP(BE, ABY, LDX)
        OP(D9, ABY, CMP)
        OP(F9, ABY, SBC)
        OP(B7, ZPY, LAX)
        OP(BF, ABY, LAX)
        OP(83, INX, SAX)
        OP(87, ZPG, SAX)
        OP(8F, ABS, SAX)
        OP(97, ZPY, SAX)
        OP(EB, IMM, SBC)
        OP(C3, INX, DCP)
        OP(C7, ZPG, DCP)
        OP(CF, ABS, DCP)
        OP(D3, INY, DCP)
        OP(D7, ZPX, DCP)
        OP(DB, ABY, DCP)
        OP(DF, ABX, DCP)
        OP(E3, INX, ISB)
        OP(E7, ZPG, ISB)
        OP(EF, ABS, ISB)
        OP(F3, INY, ISB)
        OP(F7, ZPX, ISB)
        OP(FB, ABY, ISB)
        OP(FF, ABX, ISB)
        OP(03, INX, SLO)
        OP(07, ZPG, SLO)
        OP(0F, ABS, SLO)
        OP(13, INY, SLO)
        OP(17, ZPX, SLO)
        OP(1B, ABY, SLO)
        OP(1F, ABX, SLO)
        OP(23, INX, RLA)
        OP(27, ZPG, RLA)
        OP(2F, ABS, RLA)
        OP(33, INY, RLA)
        OP(37, ZPX, RLA)
        OP(3B, ABY, RLA)
        OP(3F, ABX, RLA)
        OP(43, INX, SRE)
        OP(47, ZPG, SRE)
        OP(4F, ABS, SRE)
        OP(53, INY, SRE)
        OP(57, ZPX, SRE)
        OP(5B, ABY, SRE)
        OP(5F, ABX, SRE)
        OP(63, INX, RRA)
        OP(67, ZPG, RRA)
        OP(6F, ABS, RRA)
        OP(73, INY, RRA)
        OP(77, ZPX, RRA)
        OP(7B, ABY, RRA)
        OP(7F, ABX, RRA)
    default:
        printf("%X\n", opcode);
        assert(!"exit!");
    }
    
}


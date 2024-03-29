#ifndef SFCE_6502_H_
#define SFCE_6502_H_

#include "cpu.h"
class Addressing : public Cpu{
public:
    Addressing(Famicom*);
    uint16_t UNK();
    uint16_t ACC();
    uint16_t IMP();
    uint16_t IMM();
    uint16_t ABS();
    uint16_t ABX();
    uint16_t ABY();
    uint16_t ZPG();
    uint16_t ZPX();
    uint16_t ZPY();
    uint16_t INX();
    uint16_t INY();
    uint16_t IND();
    uint16_t REL();
};

class Operation : public Cpu{
public:
    Operation(Famicom*);
    void ADC(uint16_t);
    void AND(uint16_t);
    void ASL(uint16_t);
    void BCC(uint16_t);
    void BEQ(uint16_t);
    void BIT(uint16_t);
    void BMI(uint16_t);
    void BNE(uint16_t);
    void BPL(uint16_t);
    void BRK(uint16_t);
    void BSC(uint16_t);
    void BVC(uint16_t);
    void BVS(uint16_t);
    void CLC(uint16_t);
    void CLD(uint16_t);
    void CLI(uint16_t);
    void CLV(uint16_t);
    void CMP(uint16_t);
    void CPX(uint16_t);
    void CPY(uint16_t);
    void DEC(uint16_t);
    void DEX(uint16_t);
    void DEY(uint16_t);
    void EOR(uint16_t);
    void INC(uint16_t);
    void INY(uint16_t);
    void INX(uint16_t);
    void JMP(uint16_t);
    void JSR(uint16_t);
    void LDA(uint16_t);
    void LDX(uint16_t);
    void LDY(uint16_t);
    void LSR(uint16_t);
    void NOP(uint16_t);
    void ORA(uint16_t);
    void PHA(uint16_t);
    void PHP(uint16_t);
    void PLA(uint16_t);
    void PLP(uint16_t);
    void RTS(uint16_t);
    void RTI(uint16_t);
    void ROL(uint16_t);
    void ROR(uint16_t);
    void SBC(uint16_t);
    void SEC(uint16_t);
    void SED(uint16_t);
    void SEI(uint16_t);
    void STA(uint16_t);
    void STX(uint16_t);
    void STY(uint16_t);
    void TAX(uint16_t);
    void TAY(uint16_t);
    void TSX(uint16_t);
    void TXA(uint16_t);
    void TXS(uint16_t);
    void TYA(uint16_t);

    void ASLA(uint16_t);
    void LSRA(uint16_t);
    void ROLA(uint16_t);
    void RORA(uint16_t);

    void ALR(uint16_t);
    void ANC(uint16_t);
    void ARR(uint16_t);
    void AXS(uint16_t);
    void LAX(uint16_t);
    void SAX(uint16_t);
    void DCP(uint16_t);
    void ISB(uint16_t);
    void ISC(uint16_t);
    void RLA(uint16_t);
    void RRA(uint16_t);
    void SLO(uint16_t);
    void SRE(uint16_t);
};

#endif
#include "6502.h"

Addressing::Addressing(Famicom* fa){
    famicom_ = fa;
}
Operation::Operation(Famicom* fa){
    famicom_ = fa;
}

// Addressing
uint16_t Addressing::ACC(){
    // Op Accumulator
    // 累加器寻址 操作对象为累加器
    return 0;
}
uint16_t Addressing::IMP(){
    // Implied Addressing
    // 隐含寻址 指令中隐含地址, 无需寻址
    return 0;
}
uint16_t Addressing::IMM(){
    // Immediate Addressing
    // 立即寻址
    const uint16_t address = REG_PC;
    REG_PC++;
    return (uint16_t) address;
}
uint16_t Addressing::ABS(){
    // Absolute Addressing
    // 绝对寻址
    const uint8_t address0 = Read(REG_PC++);
    const uint8_t address1 = Read(REG_PC++);
    return (uint16_t) address0 | (uint16_t) ((uint16_t) address1 << 8);
}
uint16_t Addressing::ABX(){
    // Absolute X Addressing
    // 绝对X变址
    const uint16_t base = ABS();
    return base + (uint16_t)REG_X;
}
uint16_t Addressing::ABY(){
    // Absolute Y Addressing
    // 绝对Y变址
    const uint16_t base = ABS();
    return base + (uint16_t)REG_Y;
}
uint16_t Addressing::ZPG(){
    // Zero-Page  Addressing
    // 零页寻址
    const uint8_t address = Read(REG_PC++);
    return (uint16_t) address;
}
uint16_t Addressing::ZPX(){
    // Zero-Page X Addressing
    // 零页X变址
    uint16_t base = ZPG();
    return (base + (uint16_t)REG_X) & (uint16_t)0x00FF;
}
uint16_t Addressing::ZPY(){
    // Zero-Page Y Addressing
    // 零页Y变址
    uint16_t base = ZPG();
    return (base + (uint16_t)REG_Y) & (uint16_t)0x00FF;
}
uint16_t Addressing::IND(){
    // Indirect Addressing
    // 间接寻址
    uint16_t temp1 = ABS();
    // BUG
    uint16_t temp2 = (temp1 & 0xFF00) | ((temp1+1) & 0x00FF);
    return (uint16_t) Read(temp1) | (uint16_t) ((uint16_t) Read(temp2) << 8);
}
uint16_t Addressing::INX(){
    // Pre-indexed Indirect Addressing
    // 间接X变址
    uint8_t base = Read(REG_PC++) + REG_X;
    const uint8_t address0 = Read(base++);
    const uint8_t address1 = Read(base++);
    return (uint16_t) address0 | (uint16_t) ((uint16_t) address1 << 8);
}
uint16_t Addressing::INY(){
    // Post-indexed Indirect Addressing
    // 间接Y变址
    uint8_t base = Read(REG_PC++);
    const uint8_t address0 = Read(base++);
    const uint8_t address1 = Read(base++);
    return ((uint16_t) address0 | (uint16_t) ((uint16_t) address1 << 8)) + (uint16_t)REG_Y;
}
uint16_t Addressing::REL(){
    // Relative Addressing
    // 相对寻址
    const uint8_t temp = Read(REG_PC++);
    const uint16_t address =  REG_PC + (int8_t)temp;
    return address;
}

// Operation
void Operation::ADC(uint16_t address){
    // Add with carry
    const uint8_t src = Read(address);
    const uint16_t res16 = (uint16_t)REG_A + (uint16_t)src + (REG_CF ? 1 : 0);
    REG_CF_IF(res16 >> 8);
    const uint8_t res8 = (uint8_t)res16;
    REG_VF_IF(!((REG_A ^ src) & 0x80) && ((REG_A ^ res8) & 0x80));
    REG_A = res8;
    CHECK_ZSFLAG(REG_A);
}
void Operation::AND(uint16_t address){
    // "AND" memory with accumulator 
    REG_A &= Read(address);
    CHECK_ZSFLAG(REG_A);
}
void Operation::ASL(uint16_t address){
    // Shift Left One Bit (Memory)
    uint8_t temp = Read(address);
    REG_CF_IF(temp >> 7);
    temp <<= 1;
    Write(address, temp);
    CHECK_ZSFLAG(temp);
}
void Operation::BCC(uint16_t address){
    // Branch if carry clear
    if(!REG_CF) REG_PC = address;
}
void Operation::BEQ(uint16_t address){
    // Branch if equal
    if(REG_ZF) REG_PC = address;
}
void Operation::BIT(uint16_t address){
    // BIT Test bits in memory with accumulator
    const uint8_t value = Read(address);
    REG_NF_IF((value >> 7) & 1);
    REG_VF_IF((value >> 6) & 1);
    REG_ZF_IF(!(REG_A & value));
}
void Operation::BMI(uint16_t address){
    // Branch on result minus
    if(REG_NF) REG_PC = address;
}
void Operation::BNE(uint16_t address){
    // Branch if not equal
    if(!REG_ZF) REG_PC = address;
}
void Operation::BPL(uint16_t address){
    // Branch on result plus
    if(!REG_NF) REG_PC = address;
}
void Operation::BRK(uint16_t address){
    // Forced Interrupt PC + 2 toS P toS
    REG_PC++;

}
void Operation::BSC(uint16_t address){
    // Branch if carry set
    if(REG_CF) REG_PC = address;
}
void Operation::BVC(uint16_t address){
    // Branch on overflow clear
    if(!REG_VF) REG_PC = address;
}
void Operation::BVS(uint16_t address){
    // Branch on overflow set
    if(REG_VF) REG_PC = address;
}
void Operation::CLC(uint16_t address){
    // Clear carry
    REG_CF_CL;
}
void Operation::CLI(uint16_t address){
    // Clear interrupt disable bit
    REG_IF_CL;
}
void Operation::CLD(uint16_t address){
    // Clear decimal mode
    REG_DF_CL;
}
void Operation::CLV(uint16_t address){
    // Clear overflow flag
    REG_VF_CL;
}
void Operation::CMP(uint16_t address){
    // Compare memory and accumulator
    const uint16_t res = (uint16_t)REG_A - (uint16_t)Read(address);
    REG_CF_IF(!(res & (uint16_t)0x8000));
    CHECK_ZSFLAG((uint8_t)res);
}
void Operation::CPX(uint16_t address){
    // Compare Memory and Index X
    const uint16_t res = (uint16_t)REG_X - (uint16_t)Read(address);
    REG_CF_IF(!(res & (uint16_t)0x8000));
    CHECK_ZSFLAG((uint8_t)res);

}
void Operation::CPY(uint16_t address){
    // Compare Memory and Index Y
    const uint16_t res = (uint16_t)REG_Y - (uint16_t)Read(address);
    REG_CF_IF(!(res & (uint16_t)0x8000));
    CHECK_ZSFLAG((uint8_t)res);

}
void Operation::DEC(uint16_t address){
    // DEX Decrement Memory by one
    uint8_t temp = Read(address);
    temp--;
    Write(address, temp);
    CHECK_ZSFLAG(temp);
}
void Operation::DEX(uint16_t address){
    // DEX Decrement index X by one
    REG_X--;
    CHECK_ZSFLAG(REG_X);
}
void Operation::DEY(uint16_t address){
    // DEX Decrement index Y by one
    REG_Y--;
    CHECK_ZSFLAG(REG_Y);
}
void Operation::EOR(uint16_t address){
    // "Exclusive-Or" memory with accumulator
    REG_A ^= Read(address);
    CHECK_ZSFLAG(REG_A);
}
void Operation::INC(uint16_t address){
    // Increment Memory by One
    // Operation:  M + 1 -> M 
    uint8_t temp = Read(address);
    temp++;
    Write(address, temp);
    CHECK_ZSFLAG(temp);
}
void Operation::INX(uint16_t address){
    // Increment Index X by one
    // Operation:  X + 1 -> X 
    REG_X++;
    CHECK_ZSFLAG(REG_X);
}
void Operation::INY(uint16_t address){
    // Increment Index Y by one
    // Operation:  Y + 1 -> Y 
    REG_Y++;
    CHECK_ZSFLAG(REG_Y);
}
void Operation::JMP(uint16_t address){
    REG_PC = address;
}
void Operation::JSR(uint16_t address){
    //Jump to subroutine
    REG_PC--;
    PUSH(uint8_t(REG_PC >> 8));
    PUSH(uint8_t(REG_PC & 0XFF));
    REG_PC = address;
}
void Operation::LDA(uint16_t address){
    // Load A
    REG_A = Read(address);
    CHECK_ZSFLAG(REG_A);
}
void Operation::LDX(uint16_t address){
    // Load X
    REG_X = Read(address);
    CHECK_ZSFLAG(REG_X);
}
void Operation::LDY(uint16_t address){
    // Load Y
    REG_Y = Read(address);
    CHECK_ZSFLAG(REG_Y);
}
void Operation::LSR(uint16_t address){
    // Shift Right One Bit (Memory)
    uint8_t temp = Read(address);
    REG_CF_IF(temp & 1);
    temp >>= 1;
    Write(address, temp);
    CHECK_ZSFLAG(temp);
}
void Operation::NOP(uint16_t address){
    // NOP: Do nothing.
}
void Operation::ORA(uint16_t address){
    // "OR" memory with accumulator
    REG_A |= Read(address);
    CHECK_ZSFLAG(REG_A);
}
void Operation::PHA(uint16_t address){
    // Push accumulator on stack
    PUSH(REG_A);
}
void Operation::PHP(uint16_t address){
    // Push processor status on stack
    PUSH(REG_P | (uint8_t)(FLAG_B | FLAG_R))
}
void Operation::PLA(uint16_t address){
    // Pull accumulator from stack
    REG_A = POP();
    CHECK_ZSFLAG(REG_A);
}
void Operation::PLP(uint16_t address){
    // Pull Processor Status from Stack
    REG_P = POP();
    REG_RF_SE;
    REG_BF_CL;
}

void Operation::RTS(uint16_t address){
    // Return from subroutine
    const uint8_t pcl = POP();
    const uint8_t pch = POP();
    REG_PC = (uint16_t)pcl | ((uint16_t) pch << 8);
    REG_PC++;
}
void Operation::RTI(uint16_t address){
    // Return from interrupt
    REG_P = POP();
    REG_RF_SE;
    REG_BF_CL;
    const uint8_t pcl = POP();
    const uint8_t pch = POP();
    REG_PC = (uint16_t)pcl | ((uint16_t) pch << 8);
}
void Operation::ROL(uint16_t address){
    //Rotate One Bit Left (Memory)
    uint16_t result16 = Read(address);
    result16 <<= 1;
    result16 |= ((uint16_t)REG_CF) >> (INDEX_C);
    REG_CF_IF(result16 & (uint16_t)0x100);
    Write(address, (uint8_t)result16);
    CHECK_ZSFLAG((uint8_t)result16);
}
void Operation::ROR(uint16_t address){
    // Rotate One Bit Right (Memory)
    uint16_t result16 = Read(address);
    result16 |= ((uint16_t)REG_CF) << (8 - INDEX_C);
    REG_CF_IF(result16 & 1);
    result16 >>= 1;
    Write(address, (uint8_t)result16);
    CHECK_ZSFLAG((uint8_t)result16);
}
void Operation::SBC(uint16_t address){
    // Subtract memory from accumulator with borrow
    const uint8_t src = Read(address);
    const uint16_t res16 = (uint16_t)REG_A - (uint16_t)src - (REG_CF ? 0 : 1);
    REG_CF_IF(!(res16 >> 8));
    const uint8_t res8 = (uint8_t)res16;
    REG_VF_IF(((REG_A ^ src) & 0x80) && ((REG_A ^ res8) & 0x80));
    REG_A = res8;
    CHECK_ZSFLAG(REG_A);
}
void Operation::SEC(uint16_t address){
    // Set carry flag
    REG_CF_SE;
}
void Operation::SED(uint16_t address){
    // Set decimal mode
    REG_DF_SE;
}
void Operation::SEI(uint16_t address){
    // Set interrupt disable status
    REG_IF_SE;
}
void Operation::STA(uint16_t address){
    // Store A to address
    Write(address, REG_A);
}
void Operation::STX(uint16_t address){
    // Store X To address
    Write(address, REG_X);
}
void Operation::STY(uint16_t address){
    // Store Y To address
    Write(address, REG_Y);
}
void Operation::TAX(uint16_t address){
    // Transfer accumulator to index X
    REG_X = REG_A;
    CHECK_ZSFLAG(REG_X);
}
void Operation::TAY(uint16_t address){
    // Transfer accumulator to index Y
    REG_Y = REG_A;
    CHECK_ZSFLAG(REG_Y);
}
void Operation::TSX(uint16_t address){
    // Transfer stack pointer to index X
    REG_X = REG_SP;
    CHECK_ZSFLAG(REG_X);
}
void Operation::TXA(uint16_t address){
    // Transfer index X to accumulator
    REG_A = REG_X;
    CHECK_ZSFLAG(REG_A);
}
void Operation::TXS(uint16_t address){
    // Transfer index X to stack pointer
    REG_SP = REG_X;
}
void Operation::TYA(uint16_t address){
    // Transfer index Y to accumulator
    REG_A = REG_Y;
    CHECK_ZSFLAG(REG_A);
}

void Operation::ASLA(uint16_t address){
    // Shift Left One Bit (Accumulator)
    REG_CF_IF(REG_A >> 7);
    REG_A <<= 1;
    CHECK_ZSFLAG(REG_A);
}
void Operation::LSRA(uint16_t address){
    // Shift right one bit (accumulator)
    REG_CF_IF(REG_A & 1);
    REG_A >>= 1;
    CHECK_ZSFLAG(REG_A);
}
void Operation::ROLA(uint16_t address){
    //Rotate One Bit Left (Accumulator)
    uint16_t result16 = REG_A;
    result16 <<= 1;
    result16 |= ((uint16_t)REG_CF) >> (INDEX_C);
    REG_CF_IF(result16 & (uint16_t)0x100);
    REG_A = (uint8_t)result16;
    CHECK_ZSFLAG(REG_A);
}
void Operation::RORA(uint16_t address){
    // Rotate one bit right (accumulator) 
    uint16_t result16 = REG_A;
    result16 |= ((uint16_t)REG_CF) << (8 - INDEX_C);
    REG_CF_IF(result16 & 1);
    result16 >>= 1;
    REG_A = (uint8_t)result16;
    CHECK_ZSFLAG(REG_A);
}


void Operation::DCP(uint16_t address){
    // Decrement memory then Compare with A 
    // DEC + CMP
    uint8_t temp = Read(address);
    temp--;
    Write(address, temp);
    CHECK_ZSFLAG(temp);

    const uint16_t res = (uint16_t)REG_A - (uint16_t)Read(address);
    REG_CF_IF(!(res & (uint16_t)0x8000));
    CHECK_ZSFLAG((uint8_t)res);
}
void Operation::LAX(uint16_t address){
    // Load 'A' then Transfer X
    REG_A = Read(address);
    REG_X = REG_A;
    CHECK_ZSFLAG(REG_A);
}
void Operation::SAX(uint16_t address){
    // Store A 'And' X
    Write(address, REG_A & REG_X);
}
void Operation::ISB(uint16_t address){
    // Increment memory then Subtract with Carry
    // INC + SBC
    uint8_t temp = Read(address);
    temp++;
    Write(address, temp);
    CHECK_ZSFLAG(temp);

    const uint8_t src = Read(address);
    const uint16_t res16 = (uint16_t)REG_A - (uint16_t)src - (REG_CF ? 0 : 1);
    REG_CF_IF(!(res16 >> 8));
    const uint8_t res8 = (uint8_t)res16;
    REG_VF_IF(((REG_A ^ src) & 0x80) && ((REG_A ^ res8) & 0x80));
    REG_A = res8;
    CHECK_ZSFLAG(REG_A);
}
void Operation::ISC(uint16_t address){
    // Same with ISB
    // INC + SBC
    ISB(address);
}
void Operation::SLO(uint16_t address){
    // Shift Left then 'Or'
    // ASL + ORA
    uint8_t temp = Read(address);
    REG_CF_IF(temp >> 7);
    temp <<= 1;
    Write(address, temp);
    CHECK_ZSFLAG(temp);

    REG_A |= Read(address);
    CHECK_ZSFLAG(REG_A);
}
void Operation::RLA(uint16_t address){
    // Rotate Left then 'And'
    // ROL + AND
    uint16_t result16 = Read(address);
    result16 <<= 1;
    result16 |= ((uint16_t)REG_CF) >> (INDEX_C);
    REG_CF_IF(result16 & (uint16_t)0x100);
    Write(address, (uint8_t)result16);
    CHECK_ZSFLAG((uint8_t)result16);

    REG_A &= Read(address);
    CHECK_ZSFLAG(REG_A);
}
void Operation::SRE(uint16_t address){
    // Shift Right then "Exclusive-Or"
    // LSR + EOR
    uint8_t temp = Read(address);
    REG_CF_IF(temp & 1);
    temp >>= 1;
    Write(address, temp);
    CHECK_ZSFLAG(temp);

    REG_A ^= Read(address);
    CHECK_ZSFLAG(REG_A);
}
void Operation::RRA(uint16_t address){
    // Rotate Right then Add with Carry
    // ROR + ADC
    uint16_t result16 = Read(address);
    result16 |= ((uint16_t)REG_CF) << (8 - INDEX_C);
    REG_CF_IF(result16 & 1);
    result16 >>= 1;
    Write(address, (uint8_t)result16);
    CHECK_ZSFLAG((uint8_t)result16);

    const uint8_t src = Read(address);
    const uint16_t res16 = (uint16_t)REG_A + (uint16_t)src + (REG_CF ? 1 : 0);
    REG_CF_IF(res16 >> 8);
    const uint8_t res8 = (uint8_t)res16;
    REG_VF_IF(!((REG_A ^ src) & 0x80) && ((REG_A ^ res8) & 0x80));
    REG_A = res8;
    CHECK_ZSFLAG(REG_A);
}
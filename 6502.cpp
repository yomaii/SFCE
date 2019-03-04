#include "6502.h"

Addressing::Addressing(Famicom* fa){
    famicom = fa;
}
Operation::Operation(Famicom* fa){
    famicom = fa;
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
    const uint8_t address0 = read(REG_PC++);
    const uint8_t address1 = read(REG_PC++);
    return (uint16_t) address0 | (uint16_t) ((uint16_t) address1 << 8);
}
uint16_t Addressing::ABX(){
    // Absolute X Addressing
    // 绝对X变址
    uint16_t base = ABS();
    return base + (uint16_t)REG_X;
}
uint16_t Addressing::ABY(){
    // Absolute Y Addressing
    // 绝对Y变址
    uint16_t base = ABS();
    return base + (uint16_t)REG_Y;
}
uint16_t Addressing::ZPG(){
    // Zero-Page  Addressing
    // 零页寻址
    const uint8_t address = read(REG_PC++);
    return (uint16_t) address;
}
uint16_t Addressing::ZPX(){
    // Zero-Page X Addressing
    // 零页X变址
    uint16_t base = ZPG();
    return base + (uint16_t)REG_X;
}
uint16_t Addressing::ZPY(){
    // Zero-Page Y Addressing
    // 零页Y变址
    uint16_t base = ZPG();
    return base + (uint16_t)REG_Y;
}
uint16_t Addressing::IND(){
    // Indirect Addressing
    // 间接寻址
    uint16_t temp1 = ABS();
    // BUG
    uint16_t temp2 = (temp1 & 0xFF00) | ((temp1+1) & 0x00FF);
    return (uint16_t) read(temp1) | (uint16_t) ((uint16_t) read(temp2) << 8);
}
uint16_t Addressing::INX(){
    // Pre-indexed Indirect Addressing
    // 间接X变址
    uint16_t base = (uint16_t)read(REG_PC++) + (uint16_t)REG_X;
    const uint8_t address0 = read(base++);
    const uint8_t address1 = read(base++);
    return (uint16_t) address0 | (uint16_t) ((uint16_t) address1 << 8);
}
uint16_t Addressing::INY(){
    // Post-indexed Indirect Addressing
    // 间接Y变址
    uint16_t base = (uint16_t)read(REG_PC++);
    const uint8_t address0 = read(base++);
    const uint8_t address1 = read(base++);
    return (uint16_t) address0 | (uint16_t) ((uint16_t) address1 << 8) + (uint16_t)REG_Y;
}
uint16_t Addressing::REL(){
    // Relative Addressing
    // 相对寻址
    uint8_t temp = read(REG_PC++);
    return REG_PC + (uint16_t)temp;
}



// Operation
void Operation::ADC(uint16_t address){
    // Add with carry
    const uint8_t src = read(address);
    const uint16_t res16 = (uint16_t)REG_A + (uint16_t)src + (REG_CF ? 1 : 0);
    REG_CF_IF(res16 >> 8);
    const uint8_t res8 = (uint8_t)res16;
    REG_VF_IF(!((REG_A ^ src) & 0x80) && ((REG_A ^ res8) & 0x80));
    REG_A = res8;
    CHECK_ZSFLAG(REG_A);
}
void Operation::AND(uint16_t address){
    // "AND" memory with accumulator 
    REG_A &= read(address);
    CHECK_ZSFLAG(REG_A);
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
    const uint8_t value = read(address);
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
    const uint16_t res = (uint16_t)REG_A - (uint16_t)read(address);
    REG_CF_IF(!(res & (uint16_t)0x8000));
    CHECK_ZSFLAG((uint8_t)res);

}
void Operation::CPX(uint16_t address){
    // Compare Memory and Index X
    const uint16_t res = (uint16_t)REG_X - (uint16_t)read(address);
    REG_CF_IF(!(res & (uint16_t)0x8000));
    CHECK_ZSFLAG((uint8_t)res);

}
void Operation::CPY(uint16_t address){
    // Compare Memory and Index Y
    const uint16_t res = (uint16_t)REG_Y - (uint16_t)read(address);
    REG_CF_IF(!(res & (uint16_t)0x8000));
    CHECK_ZSFLAG((uint8_t)res);

}
void Operation::EOR(uint16_t address){
    // "Exclusive-Or" memory with accumulator
    REG_A ^= read(address);
    CHECK_ZSFLAG(REG_A);
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
    REG_A = read(address);
    CHECK_ZSFLAG(REG_A);
}
void Operation::LDX(uint16_t address){
    // Load X
    REG_X = read(address);
    CHECK_ZSFLAG(REG_X);
}
void Operation::LDY(uint16_t address){
    // Load Y
    REG_Y = read(address);
    CHECK_ZSFLAG(REG_Y);
}
void Operation::NOP(uint16_t address){
    // NOP: Do nothing.
}
void Operation::ORA(uint16_t address){
    // "OR" memory with accumulator
    REG_A |= read(address);
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
void Operation::ROLA(uint16_t address){
    uint16_t result16 = REG_A;
    result16 <<= 1;
    result16 |= ((uint16_t)REG_CF) >> (statusIndex::INDEX_C);
    REG_CF_IF(result16 & (uint16_t)0x100);
    REG_A = (uint8_t)result16;
    CHECK_ZSFLAG(REG_A);
}
void Operation::RTS(uint16_t address){
    // Return from subroutine
    const uint8_t pcl = POP();
    const uint8_t pch = POP();
    REG_PC = (uint16_t)pcl | ((uint16_t) pch << 8);
    REG_PC++;
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
    write(address, REG_A);
}
void Operation::STX(uint16_t address){
    // Store X To address
    write(address, REG_X);
}
void Operation::STY(uint16_t address){
    // Store Y To address
    write(address, REG_Y);
}




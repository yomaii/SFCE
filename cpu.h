#ifndef SFCE_CPU_H_
#define SFCE_CPU_H_

#include <cstdint>
#include <string>
#include <iostream>
#include "code.h"
using std::string;
// status index
enum
{
    INDEX_C = 0,
    INDEX_Z = 1,
    INDEX_I = 2,
    INDEX_D = 3,
    INDEX_B = 4,
    INDEX_R = 5,
    INDEX_V = 6,
    INDEX_S = 7,
    INDEX_N = INDEX_S,
};

// status flag
enum
{
    FLAG_C = 1 << 0,    // 进位标记(Carry flag)
    FLAG_Z = 1 << 1,    // 零标记 (Zero flag)
    FLAG_I = 1 << 2,    // 禁止中断(Irq disabled flag)
    FLAG_D = 1 << 3,    // 十进制模式(Decimal mode flag)
    FLAG_B = 1 << 4,    // 软件中断(BRK flag)
    FLAG_R = 1 << 5,    // 保留标记(Reserved), should be keep 1
    FLAG_V = 1 << 6,    // 溢出标记(Overflow  flag)
    FLAG_S = 1 << 7,    // 符号标记(Sign flag)
    FLAG_N = FLAG_S,    // (Negative Flag)
};

struct CpuRegister{
    // 指令计数器 Program Counter
    uint16_t    programCounter;
    // 状态寄存器 Status Register
    uint8_t     status;
    // 累加寄存器 Accumulator
    uint8_t     accumulator;
    // X 变址寄存器 X Index Register
    uint8_t     xIndex;
    // Y 变址寄存器 Y Index Register
    uint8_t     yIndex;
    // 栈指针  Stack Pointer
    uint8_t     stackPointer;
    // 保留对齐用
    uint8_t     unused;
};
// cpu vector
enum
{
    CPU_NMI     = 0xFFFA,
    CPU_RESET   = 0xFFFC,
    CPU_IRQBRK  = 0xFFFE
};

class Famicom;

class Cpu
{
private:
    Famicom* famicom_;
    friend class Addressing;
    friend class Operation;
    Cpu();
public:
    Cpu(Famicom&);
    uint8_t Read(uint16_t);
    void Write(uint16_t, uint8_t);
    uint8_t ReadPPU(uint16_t);
    void WritePPU(uint16_t, uint8_t);
    uint8_t Read4020(uint16_t);
    void Write4020(uint16_t, uint8_t);
    string Disassembly(uint16_t address);
    string btoh(uint8_t);
    string btod(uint8_t);
    void ExecuteOne();
    void Log();
    void NMI();
};
#define REG (famicom_->registers_)
#define REG_PC (REG.programCounter)
#define REG_SP (REG.stackPointer)
#define REG_A (REG.accumulator)
#define REG_X (REG.xIndex)
#define REG_Y (REG.yIndex)
#define REG_P (REG.status)

// if中判断用FLAG
#define REG_CF (REG_P & (uint8_t)FLAG_C)
#define REG_ZF (REG_P & (uint8_t)FLAG_Z)
#define REG_IF (REG_P & (uint8_t)FLAG_I)
#define REG_DF (REG_P & (uint8_t)FLAG_D)
#define REG_BF (REG_P & (uint8_t)FLAG_B)
#define REG_VF (REG_P & (uint8_t)FLAG_V)
#define REG_SF (REG_P & (uint8_t)FLAG_S)
#define REG_NF (REG_P & (uint8_t)FLAG_N)
// 将FLAG将变为1
#define REG_CF_SE (REG_P |= (uint8_t)FLAG_C)
#define REG_ZF_SE (REG_P |= (uint8_t)FLAG_Z)
#define REG_IF_SE (REG_P |= (uint8_t)FLAG_I)
#define REG_DF_SE (REG_P |= (uint8_t)FLAG_D)
#define REG_BF_SE (REG_P |= (uint8_t)FLAG_B)
#define REG_RF_SE (REG_P |= (uint8_t)FLAG_R)
#define REG_VF_SE (REG_P |= (uint8_t)FLAG_V)
#define REG_SF_SE (REG_P |= (uint8_t)FLAG_S)
#define REG_NF_SE (REG_P |= (uint8_t)FLAG_N)
// 将FLAG将变为0
#define REG_CF_CL (REG_P &= ~(uint8_t)FLAG_C)
#define REG_ZF_CL (REG_P &= ~(uint8_t)FLAG_Z)
#define REG_IF_CL (REG_P &= ~(uint8_t)FLAG_I)
#define REG_DF_CL (REG_P &= ~(uint8_t)FLAG_D)
#define REG_BF_CL (REG_P &= ~(uint8_t)FLAG_B)
#define REG_VF_CL (REG_P &= ~(uint8_t)FLAG_V)
#define REG_SF_CL (REG_P &= ~(uint8_t)FLAG_S)
#define REG_NF_CL (REG_P &= ~(uint8_t)FLAG_N)
// 将FLAG将变为0或者1
#define REG_CF_IF(x) (x ? REG_CF_SE : REG_CF_CL);
#define REG_ZF_IF(x) (x ? REG_ZF_SE : REG_ZF_CL);
#define REG_OF_IF(x) (x ? REG_IF_SE : REG_IF_CL);
#define REG_DF_IF(x) (x ? REG_DF_SE : REG_DF_CL);
#define REG_BF_IF(x) (x ? REG_BF_SE : REG_BF_CL);
#define REG_VF_IF(x) (x ? REG_VF_SE : REG_VF_CL);
#define REG_SF_IF(x) (x ? REG_SF_SE : REG_SF_CL);
#define REG_NF_IF(x) (x ? REG_NF_SE : REG_NF_CL);

#define PUSH(a) (famicom_->main_memory_ + 0x100)[REG_SP--] = a;
#define POP() (famicom_->main_memory_ + 0x100)[++REG_SP];
#define CHECK_ZSFLAG(x) { REG_SF_IF(x & (uint8_t)0x80); REG_ZF_IF(x == 0); }

#include "famicom.h"
#include "6502.h"
#endif
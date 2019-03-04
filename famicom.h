#pragma once

#include "rom.h"
#include "code.h"
#include "cpu.h"
#include <string>
using namespace std;


class Famicom
{
private:
    Rom rom;
    bool loaded = false;
    cpuRegister registers;
    uint8_t*    prgBanks[0x10000 >> 13];
    uint8_t    saveMemory[8 * 1024];
    uint8_t    mainMemory[2 * 1024];

    friend class Cpu;
    friend class Addressing;
    friend class Operation;
public:
    errorCode init(string);
    errorCode load(string);
    errorCode resetMapper_00();
    errorCode reset();
    void loadProgram8k(int, int);
    void showInfo();
    void release();
};
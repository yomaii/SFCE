#ifndef SFCE_FAMICOM_H_
#define SFCE_FAMICOM_H_
#include <cstdint>
#include <string>
#include "code.h"
#include "cpu.h"
using namespace std;

struct Rom
{
    // PRG-ROM
    uint8_t*    prg;
    // CHR-ROM
    uint8_t*    chr;
    // 16KB PRG size counter
    uint32_t    count_16k;
    // 8KB CHR size counter
    uint32_t    count_8k;
    // Mapper number
    uint8_t     mapper_number;
    // Vertical Mirroring
    uint8_t     vmirroring;
    // Four-screen mode
    uint8_t     four_screen;
    // Save ram
    uint8_t     save_ram;
    // Reserved
    uint8_t     reserved[4];
};

struct PPU
{
    uint8_t* banks[0x4000 / 0x0400];
    uint16_t vramaddr;
    uint8_t  ctrl;
    uint8_t  mask;
    uint8_t  status;
    uint8_t  oamaddr;
    uint8_t  scroll[2];
    uint8_t  writex2;
    uint8_t  pseudo;
    uint8_t  spindexes[0x20];
    uint8_t  sprites[0x100];
};
// ppu flag
enum
{
    PPU2000_NMIGen  = 0x80, // [0x2000]VBlank期间是否产生NMI
    PPU2000_Sp8x16  = 0x20, // [0x2000]精灵为8x16(1), 还是8x8(0)
    PPU2000_BgTabl  = 0x10, // [0x2000]背景调色板表地址$1000(1), $0000(0)
    PPUFLAG_SpTabl  = 0x08, // [0x2000]精灵调色板表地址$1000(1), $0000(0), 8x16模式下被忽略
    PPU2000_VINC32  = 0x04, // [0x2000]VRAM读写增加值32(1), 1(0)
        
    PPU2002_VBlank  = 0x80, // [0x2002]垂直空白间隙标志
    PPU2002_Sp0Hit  = 0x40, // [0x2002]零号精灵命中标志
    PPU2002_SpOver  = 0x20, // [0x2002]精灵溢出标志
};

class Famicom
{
private:
    /* physical parts */
    Rom rom_;
    
    uint8_t*   prg_banks_[0x10000 >> 13];
    uint8_t    save_memory_[8 * 1024];
    uint8_t    video_memory_[2 * 1024];
    uint8_t    video_memory_ex_[2 * 1024];
    uint8_t    main_memory_[2 * 1024];

    /* registers and status */
    bool loaded_ = false;
    CpuRegister registers_;
    uint16_t controller1_;
    uint16_t controller2_;
    uint16_t controller_status_mask_;
    uint8_t  controller_states_[16];

    /* set friend class */
    friend class Cpu;
    friend class Addressing;
    friend class Operation;
    friend void UserInput(int index, unsigned char data);
public:
    Cpu *cpu_;
    PPU ppu_;
    int Init(string romfile);
    int LoadRom(string romfile);
    void ShowInfo();
    void LoadProgram8k(int des, int src);
    void LoadChrrom1k(int des, int src);
    int Reset();
    int ResetMapper00();
    void SetupNametableBank();
    uint8_t ReadPPU(uint16_t);
    void WritePPU(uint16_t, uint8_t);
    void sVblank();
    void eVblank();
};

struct NesHeader{
    uint32_t    id;
    uint8_t     count_16k;
    uint8_t     count_8k;
    uint8_t     flag6;
    uint8_t     flag7;
    uint8_t     reserved[8];
};


#endif
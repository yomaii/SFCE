#include "famicom.h"
#include <assert.h>
#include <fstream>
#include <iostream>
using namespace std;

int Famicom::Init(string romfile){
    // set banks
    prg_banks_[0] = main_memory_;
    prg_banks_[3] = save_memory_;

    // load rom
    auto code = LoadRom(romfile);
    if(code != 0) return code;

    cpu_ = new Cpu(*this);
    
    return Reset();
}

int Famicom::LoadRom(string romfile){
    ifstream romfile_if(romfile, ios::binary | ios::in);
    if(!romfile_if) return ERROR_FILE_NOT_EXIST;

    NesHeader file_header;
    romfile_if.read((char*)&file_header, sizeof(file_header));

    union {uint32_t u32; uint8_t id[4];} header_prefix;
    header_prefix.id[0] = 'N';
    header_prefix.id[1] = 'E';
    header_prefix.id[2] = 'S';
    header_prefix.id[3] = '\x1A';

    if(file_header.id == header_prefix.u32){
        size_t prg_size = 16 * 1024 * file_header.count_16k;
        size_t chr_size = 8 * 1024 * file_header.count_8k;
        uint8_t *ptr = new uint8_t[prg_size + chr_size];
        if(ptr){
            // skip trainer for now(To Do)
            if(file_header.flag6 & ROM_TRAINER) romfile_if.seekg(512, ios::cur);
            // load rom from file
            romfile_if.read((char*)ptr, prg_size + chr_size);
            // write info to rom_
            rom_.prg = ptr;
            rom_.chr = ptr + prg_size;
            rom_.count_16k = file_header.count_16k;
            rom_.count_8k = file_header.count_8k;
            rom_.mapper_number = (file_header.flag6 >> 4) | (file_header.flag7 & 0xf0);
            rom_.vmirroring = (file_header.flag6 & ROM_VMIRROR) > 0;
            rom_.four_screen = (file_header.flag6 & ROM_4SCREEN) > 0;
            rom_.save_ram = (file_header.flag6 & ROM_SAVERAM) > 0;

            // end of load
            loaded_ = true;

            // not supported for now
            assert(!(file_header.flag6 & ROM_TRAINER) && "unsupported for now");
            assert(!(file_header.flag7 & ROM_VS_UNISYSTEM) && "unsupported for now");
            assert(!(file_header.flag7 & ROM_Playchoice10) && "unsupported for now");


        } else return ERROR_OUT_OF_MEMORY;
    } else return ERROR_ILLEGAL_FILE;

    return 0;
}

void Famicom::ShowInfo(){
    cout << "rom loaded:" << loaded_ << endl;
    if(loaded_){
        printf(
            "ROM: PRG-ROM: %d x 16kb   CHR-ROM %d x 8kb   Mapper: %03d\n", 
            (int)rom_.count_16k,
            (int)rom_.count_8k,
            (int)rom_.mapper_number
        );
    }
}

void Famicom::LoadProgram8k(int des, int src){
    prg_banks_[4 + des] = rom_.prg + 8 * 1024 * src;

}
void Famicom::LoadChrrom1k(int des, int src){
    ppu_.banks[des] = rom_.chr + 1024 * src;
}

int Famicom::ResetMapper00(){
    assert(rom_.count_16k && "bad count");
    assert(rom_.count_16k <= 2 && "bad count");

    const int id2 = rom_.count_16k & 2;

    LoadProgram8k(0, 0);
    LoadProgram8k(1, 1);
    LoadProgram8k(2, id2 + 0);
    LoadProgram8k(3, id2 + 1);

    for (int i = 0; i != 8; ++i) 
        LoadChrrom1k(i, i);

    return 0;
}
void Famicom::SetupNametableBank(){
    // 4屏
    if (rom_.four_screen) {
        ppu_.banks[0x8] = video_memory_ + 0x400 * 0;
        ppu_.banks[0x9] = video_memory_ + 0x400 * 1;
        ppu_.banks[0xa] = video_memory_ex_ + 0x400 * 0;
        ppu_.banks[0xb] = video_memory_ex_ + 0x400 * 1;
    }
    // 横版
    else if (rom_.vmirroring) {
        ppu_.banks[0x8] = video_memory_ + 0x400 * 0;
        ppu_.banks[0x9] = video_memory_ + 0x400 * 1;
        ppu_.banks[0xa] = video_memory_ + 0x400 * 0;
        ppu_.banks[0xb] = video_memory_ + 0x400 * 1;
    }
    // 纵版
    else {
        ppu_.banks[0x8] = video_memory_ + 0x400 * 0;
        ppu_.banks[0x9] = video_memory_ + 0x400 * 0;
        ppu_.banks[0xa] = video_memory_ + 0x400 * 1;
        ppu_.banks[0xb] = video_memory_ + 0x400 * 1;
    }
}
int Famicom::Reset(){
    // only support mapper 000 for now
    auto code = ResetMapper00();
    if(code != 0) return code;

    const uint8_t pcl = cpu_->Read(CPU_RESET + 0);
    const uint8_t pch = cpu_->Read(CPU_RESET + 1);

    registers_.programCounter = (uint16_t)pcl | (uint16_t)pch << 8;
    registers_.accumulator = 0;
    registers_.xIndex = 0;
    registers_.yIndex = 0;
    registers_.stackPointer = 0xfd;
    registers_.status = 0x34 | FLAG_R;

    SetupNametableBank();

    ppu_.banks[0xc] = ppu_.banks[0x8];
    ppu_.banks[0xd] = ppu_.banks[0x9];
    ppu_.banks[0xe] = ppu_.banks[0xa];
    ppu_.banks[0xf] = ppu_.banks[0xb];

    // for testrom (nestest.nes)
    //registers_.programCounter = 0xC000;

    return 0;
}

uint8_t Famicom::ReadPPU(uint16_t address){
    const uint16_t realAddress = address & (uint16_t) 0x3FFF;
    // bank
    if(realAddress < (uint16_t) 0x3F00){
        const uint16_t index = realAddress >> 10;
        const uint16_t offset = realAddress & (uint16_t)0x3FF;
        assert(ppu_.banks[index]);
        const uint8_t data = ppu_.pseudo;
        ppu_.pseudo = ppu_.banks[index][offset];
        return data;
    }
    // palette
    else return ppu_.pseudo = ppu_.spindexes[realAddress & (uint16_t)0x1f];
}
void Famicom::WritePPU(uint16_t address, uint8_t data){
    const uint16_t realAddress = address & (uint16_t)0x3FFF;

    if (realAddress < (uint16_t)0x3F00) {
        assert(realAddress >= 0x2000);
        const uint16_t index = realAddress >> 10;
        const uint16_t offset = realAddress & (uint16_t)0x3FF;
        assert(ppu_.banks[index]);
        ppu_.banks[index][offset] = data;
    }
    else {
        // 独立地址
        if (realAddress & (uint16_t)0x03) {
            ppu_.spindexes[realAddress & (uint16_t)0x1f] = data;
        }
        // $3F00/$3F04/$3F08/$3F0C
        else {
            const uint16_t offset = realAddress & (uint16_t)0x0f;
            ppu_.spindexes[offset] = data;
            ppu_.spindexes[offset | (uint16_t)0x10] = data;
        }
    }
}

void Famicom::sVblank(){
    ppu_.status |= (uint8_t)PPU2002_VBlank;
}

void Famicom::eVblank(){
    ppu_.status &= ~(uint8_t)PPU2002_VBlank;
}


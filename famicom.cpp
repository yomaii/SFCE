#include "famicom.h"
#include <assert.h>
#include <fstream>
#include <iostream>
using namespace std;

errorCode Famicom::init(string fileName){
    // set banks
    prgBanks[0] = mainMemory;
    prgBanks[3] = saveMemory;

    // load rom
    auto code = load(fileName);
    if(code != ERROR_OK) return code;
    return reset();
}

errorCode Famicom::load(string fileName){
    ifstream romFile(fileName, ios::binary | ios::in);
    if(!romFile) return ERROR_FILE_NOT_EXIST;
    NesHeader header;
    romFile.read((char*)&header, sizeof(header));
    union {uint32_t u32; uint8_t id[4];} headerId;
    headerId.id[0] = 'N';
    headerId.id[1] = 'E';
    headerId.id[2] = 'S';
    headerId.id[3] = '\x1A';
    if(header.id == headerId.u32){
        size_t prgSize = 16 * 1024 * header.count_16k;
        size_t chrSize =  8 * 1024 * header.count_8k;
        uint8_t* ptr = new uint8_t[prgSize + chrSize];
        if(ptr){
            if(header.flag6 & NES_TRAINER){
                // skip trainer now.
                romFile.seekg(512, ios::cur);
            }
            romFile.read((char*)ptr, prgSize + chrSize);
            // write info to rom
            printf("%X\n", *ptr);
            rom.prg = ptr;
            rom.chr = ptr + prgSize;
            rom.count_16k = header.count_16k;
            rom.count_8k = header.count_8k;
            rom.mapper_number = (header.flag6 >> 4) | (header.flag7 & 0xf0);
            rom.vmirroring = (header.flag6 & NES_VMIRROR) > 0;
            rom.four_screen = (header.flag6 & NES_4SCREEN) > 0;
            rom.save_ram = (header.flag6 & NES_SAVERAM) > 0;

            loaded = true;

            assert(!(header.flag6 & NES_TRAINER) && "unsupported");
            assert(!(header.flag7 & NES_VS_UNISYSTEM) && "unsupported");
            assert(!(header.flag7 & NES_Playchoice10) && "unsupported");

        } else return ERROR_OUT_OF_MEMORY;
    } else return ERROR_ILLEGAL_FILE;
    return ERROR_OK;
}

void Famicom::loadProgram8k(int des, int src){
    prgBanks[4 + des] = rom.prg + 8 * 1024 * src;

}
void Famicom::loadChrrom1k(int des, int src){
    ppu.banks[des] = rom.chr + 1024 * src;
}
errorCode Famicom::resetMapper_00(){
    assert(rom.count_16k && "bad count");
    assert(rom.count_16k <= 2 && "bad count");

    const int id2 = rom.count_16k & 2;

    loadProgram8k(0, 0);
    loadProgram8k(1, 1);
    loadProgram8k(2, id2 + 0);
    loadProgram8k(3, id2 + 1);

    for (int i = 0; i != 8; ++i) 
        loadChrrom1k(i, i);

    return ERROR_OK;
}
errorCode Famicom::reset(){
    errorCode code =  resetMapper_00();
    if(code) return code;
    Cpu cpu(*this);
    const uint8_t pcl = cpu.read(cpuVector::RESET + 0);
    const uint8_t pch = cpu.read(cpuVector::RESET + 1);
    registers.programCounter = (uint16_t)pcl | (uint16_t)pch << 8;
    registers.accumulator = 0;
    registers.xIndex = 0;
    registers.yIndex = 0;
    registers.stackPointer = 0xfd;
    registers.status = 0x34 | FLAG_R;

    setupNametableBank();

    ppu.banks[0xc] = ppu.banks[0x8];
    ppu.banks[0xd] = ppu.banks[0x9];
    ppu.banks[0xe] = ppu.banks[0xa];
    ppu.banks[0xf] = ppu.banks[0xb];


    // 测试指令ROM(nestest.nes)
    //registers.programCounter = 0xC000;
    return errorCode::ERROR_OK;
}
void Famicom::showInfo(){
    cout << "rom loaded:" << loaded << endl;
    if(loaded){
        printf(
            "ROM: PRG-ROM: %d x 16kb   CHR-ROM %d x 8kb   Mapper: %03d\n", 
            (int)rom.count_16k,
            (int)rom.count_8k,
            (int)rom.mapper_number
        );
    }
}

void Famicom::release(){
    if(loaded){
        delete[] rom.prg;
        rom.prg = NULL;
        loaded = false;
    }
}

uint8_t Famicom::readPPU(uint16_t address){
    const uint16_t realAddress = address & (uint16_t) 0x3FFF;
    // bank
    if(realAddress < (uint16_t) 0x3F00){
        const uint16_t index = realAddress >> 10;
        const uint16_t offset = realAddress & (uint16_t)0x3FF;
        assert(ppu.banks[index]);
        const uint8_t data = ppu.pseudo;
        ppu.pseudo = ppu.banks[index][offset];
        return data;
    }
    // palette
    else return ppu.pseudo = ppu.spindexes[realAddress & (uint16_t)0x1f];
}
void Famicom::writePPU(uint16_t address, uint8_t data){
    const uint16_t realAddress = address & (uint16_t)0x3FFF;

    if (realAddress < (uint16_t)0x3F00) {
        assert(realAddress >= 0x2000);
        const uint16_t index = realAddress >> 10;
        const uint16_t offset = realAddress & (uint16_t)0x3FF;
        assert(ppu.banks[index]);
        ppu.banks[index][offset] = data;
    }
    else {
        // 独立地址
        if (realAddress & (uint16_t)0x03) {
            ppu.spindexes[realAddress & (uint16_t)0x1f] = data;
        }
        // $3F00/$3F04/$3F08/$3F0C
        else {
            const uint16_t offset = realAddress & (uint16_t)0x0f;
            ppu.spindexes[offset] = data;
            ppu.spindexes[offset | (uint16_t)0x10] = data;
        }
    }
}

void Famicom::sVblank(){
    ppu.status |= (uint8_t)PPU2002_VBlank;
}

void Famicom::eVblank(){
    ppu.status &= ~(uint8_t)PPU2002_VBlank;
}
void Famicom::setupNametableBank(){
    // 4屏
    if (rom.four_screen) {
        ppu.banks[0x8] = videoMemory + 0x400 * 0;
        ppu.banks[0x9] = videoMemory + 0x400 * 1;
        ppu.banks[0xa] = videoMemoryEX + 0x400 * 0;
        ppu.banks[0xb] = videoMemoryEX + 0x400 * 1;
    }
    // 横版
    else if (rom.vmirroring) {
        ppu.banks[0x8] = videoMemory + 0x400 * 0;
        ppu.banks[0x9] = videoMemory + 0x400 * 1;
        ppu.banks[0xa] = videoMemory + 0x400 * 0;
        ppu.banks[0xb] = videoMemory + 0x400 * 1;
    }
    // 纵版
    else {
        ppu.banks[0x8] = videoMemory + 0x400 * 0;
        ppu.banks[0x9] = videoMemory + 0x400 * 0;
        ppu.banks[0xa] = videoMemory + 0x400 * 1;
        ppu.banks[0xb] = videoMemory + 0x400 * 1;
    }
}
#include "famicom.h"
#include <assert.h>
#include <fstream>
#include <iostream>
using namespace std;

int Famicom::init(string fileName){
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
        	if(header.flag6 && NES_TRAINER){
        		// skip trainer now.
        		romFile.seekg(512, ios::cur);
        	}
        	romFile.read((char*)ptr, prgSize + chrSize);
        	// write info to rom
        	rom.prg = ptr;
        	rom.chr = ptr + prgSize;
        	rom.count_16k = header.count_16k;
        	rom.count_8k = header.count_8k;
        	rom.mapper_number = (header.flag6 >> 4) | (header.flag7 & 0xf0);
        	rom.vmirroring = header.flag6 & NES_VMIRROR;
        	rom.four_screen = header.flag6 & NES_4SCREEN;
        	rom.save_ram = header.flag6 & NES_SAVERAM;

        	loaded = true;

            assert(!(header.flag6 & NES_TRAINER) && "unsupported");
            assert(!(header.flag6 & NES_VS_UNISYSTEM) && "unsupported");
            assert(!(header.flag6 & NES_Playchoice10) && "unsupported");

        } else return ERROR_OUT_OF_MEMORY;
    } else return ERROR_ILLEGAL_FILE;
    return ERROR_OK;
}

void Famicom::showInfo(){
	cout << "rom loaded:" << loaded << endl;
	if(loaded) cout << "PRG: " << rom.count_16k << " x 16kb  CHR: " << rom.count_8k << " x 8kb  Mapper: " << (int)rom.mapper_number << endl;
}
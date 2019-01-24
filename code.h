#pragma once

enum errorCode {
    ERROR_OK = 0,
    ERROR_FAILED,
    ERROR_FILE_NOT_EXIST,
    ERROR_ILLEGAL_FILE,
    ERROR_OUT_OF_MEMORY
};

// ROM control byte #1
enum {
    NES_VMIRROR = 0x01, 
    NES_SAVERAM = 0x02,
    NES_TRAINER = 0x04,
    NES_4SCREEN = 0x08
};

// ROM control byte #2
enum { 
    NES_VS_UNISYSTEM  = 0x01,
    NES_Playchoice10 = 0x02
};
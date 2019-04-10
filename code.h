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

union Code6502{
    uint32_t data;
    struct{
        uint8_t op;
        uint8_t a1;
        uint8_t a2;
        uint8_t ctrl;
    };

};
struct OpName {
    char        name[3];
    uint8_t     mode;
};
enum{
    AM_UNK = 0,     // 未知寻址
    AM_ACC,         // 累加器寻址: Op Accumulator
    AM_IMP,         // 隐含 寻址: Implied    Addressing
    AM_IMM,         // 立即 寻址: Immediate  Addressing
    AM_ABS,         // 绝对 寻址: Absolute   Addressing
    AM_ABX,         // 绝对X变址: Absolute X Addressing
    AM_ABY,         // 绝对Y变址: Absolute Y Addressing
    AM_ZPG,         // 零页 寻址: Zero-Page  Addressing
    AM_ZPX,         // 零页X变址: Zero-PageX Addressing
    AM_ZPY,         // 零页Y变址: Zero-PageY Addressing
    AM_INX,         // 间接X变址: Pre-indexed Indirect Addressing
    AM_INY,         // 间接Y变址: Post-indexed Indirect Addressing
    AM_IND,         // 间接 寻址: Indirect   Addressing
    AM_REL,         // 相对 寻址: Relative   Addressing
};

static const char HEXDATA[] = "0123456789ABCDEF";
static const OpName OPNAMEDATA[256] = {
    { 'B', 'R', 'K', AM_IMP },
    { 'O', 'R', 'A', AM_INX },
    { 'S', 'T', 'P', AM_UNK },
    { 'S', 'L', 'O', AM_INX },
    { 'N', 'O', 'P', AM_ZPG },
    { 'O', 'R', 'A', AM_ZPG },
    { 'A', 'S', 'L', AM_ZPG },
    { 'S', 'L', 'O', AM_ZPG },
    { 'P', 'H', 'P', AM_IMP },
    { 'O', 'R', 'A', AM_IMM },
    { 'A', 'S', 'L', AM_ACC },
    { 'A', 'N', 'C', AM_IMM },
    { 'N', 'O', 'P', AM_ABS },
    { 'O', 'R', 'A', AM_ABS },
    { 'A', 'S', 'L', AM_ABS },
    { 'S', 'L', 'O', AM_ABS },

    { 'B', 'P', 'L', AM_REL },
    { 'O', 'R', 'A', AM_INY },
    { 'S', 'T', 'P', AM_UNK },
    { 'S', 'L', 'O', AM_INY },
    { 'N', 'O', 'P', AM_ZPX },
    { 'O', 'R', 'A', AM_ZPX },
    { 'A', 'S', 'L', AM_ZPX },
    { 'S', 'L', 'O', AM_ZPX },
    { 'C', 'L', 'C', AM_IMP },
    { 'O', 'R', 'A', AM_ABY },
    { 'N', 'O', 'P', AM_IMP },
    { 'S', 'L', 'O', AM_ABY },
    { 'N', 'O', 'P', AM_ABX },
    { 'O', 'R', 'A', AM_ABX },
    { 'A', 'S', 'L', AM_ABX },
    { 'S', 'L', 'O', AM_ABX },

    { 'J', 'S', 'R', AM_ABS },
    { 'A', 'N', 'D', AM_INX },
    { 'S', 'T', 'P', AM_UNK },
    { 'R', 'L', 'A', AM_INX },
    { 'B', 'I', 'T', AM_ZPG },
    { 'A', 'N', 'D', AM_ZPG },
    { 'R', 'O', 'L', AM_ZPG },
    { 'R', 'L', 'A', AM_ZPG },
    { 'P', 'L', 'P', AM_IMP },
    { 'A', 'N', 'D', AM_IMM },
    { 'R', 'O', 'L', AM_ACC },
    { 'A', 'N', 'C', AM_IMM },
    { 'B', 'I', 'T', AM_ABS },
    { 'A', 'N', 'D', AM_ABS },
    { 'R', 'O', 'L', AM_ABS },
    { 'R', 'L', 'A', AM_ABS },

    { 'B', 'M', 'I', AM_REL },
    { 'A', 'N', 'D', AM_INY },
    { 'S', 'T', 'P', AM_UNK },
    { 'R', 'L', 'A', AM_INY },
    { 'N', 'O', 'P', AM_ZPX },
    { 'A', 'N', 'D', AM_ZPX },
    { 'R', 'O', 'L', AM_ZPX },
    { 'R', 'L', 'A', AM_ZPX },
    { 'S', 'E', 'C', AM_IMP },
    { 'A', 'N', 'D', AM_ABY },
    { 'N', 'O', 'P', AM_IMP },
    { 'R', 'L', 'A', AM_ABY },
    { 'N', 'O', 'P', AM_ABX },
    { 'A', 'N', 'D', AM_ABX },
    { 'R', 'O', 'L', AM_ABX },
    { 'R', 'L', 'A', AM_ABX },

    { 'R', 'T', 'I', AM_IMP },
    { 'E', 'O', 'R', AM_INX },
    { 'S', 'T', 'P', AM_UNK },
    { 'S', 'R', 'E', AM_INX },
    { 'N', 'O', 'P', AM_ZPG },
    { 'E', 'O', 'R', AM_ZPG },
    { 'L', 'S', 'R', AM_ZPG },
    { 'S', 'R', 'E', AM_ZPG },
    { 'P', 'H', 'A', AM_IMP },
    { 'E', 'O', 'R', AM_IMM },
    { 'L', 'S', 'R', AM_ACC },
    { 'A', 'S', 'R', AM_IMM },
    { 'J', 'M', 'P', AM_ABS },
    { 'E', 'O', 'R', AM_ABS },
    { 'L', 'S', 'R', AM_ABS },
    { 'S', 'R', 'E', AM_ABS },

    { 'B', 'V', 'C', AM_REL },
    { 'E', 'O', 'R', AM_INY },
    { 'S', 'T', 'P', AM_UNK },
    { 'S', 'R', 'E', AM_INY },
    { 'N', 'O', 'P', AM_ZPX },
    { 'E', 'O', 'R', AM_ZPX },
    { 'L', 'S', 'R', AM_ZPX },
    { 'S', 'R', 'E', AM_ZPX },
    { 'C', 'L', 'I', AM_IMP },
    { 'E', 'O', 'R', AM_ABY },
    { 'N', 'O', 'P', AM_IMP },
    { 'S', 'R', 'E', AM_ABY },
    { 'N', 'O', 'P', AM_ABX },
    { 'E', 'O', 'R', AM_ABX },
    { 'L', 'S', 'R', AM_ABX },
    { 'S', 'R', 'E', AM_ABX },

    { 'R', 'T', 'S', AM_IMP },
    { 'A', 'D', 'C', AM_INX },
    { 'S', 'T', 'P', AM_UNK },
    { 'R', 'R', 'A', AM_INX },
    { 'N', 'O', 'P', AM_ZPG },
    { 'A', 'D', 'C', AM_ZPG },
    { 'R', 'O', 'R', AM_ZPG },
    { 'R', 'R', 'A', AM_ZPG },
    { 'P', 'L', 'A', AM_IMP },
    { 'A', 'D', 'C', AM_IMM },
    { 'R', 'O', 'R', AM_ACC },
    { 'A', 'R', 'R', AM_IMM },
    { 'J', 'M', 'P', AM_IND },
    { 'A', 'D', 'C', AM_ABS },
    { 'R', 'O', 'R', AM_ABS },
    { 'R', 'R', 'A', AM_ABS },

    { 'B', 'V', 'S', AM_REL },
    { 'A', 'D', 'C', AM_INY },
    { 'S', 'T', 'P', AM_UNK },
    { 'R', 'R', 'A', AM_INY },
    { 'N', 'O', 'P', AM_ZPX },
    { 'A', 'D', 'C', AM_ZPX },
    { 'R', 'O', 'R', AM_ZPX },
    { 'R', 'R', 'A', AM_ZPX },
    { 'S', 'E', 'I', AM_IMP },
    { 'A', 'D', 'C', AM_ABY },
    { 'N', 'O', 'P', AM_IMP },
    { 'R', 'R', 'A', AM_ABY },
    { 'N', 'O', 'P', AM_ABX },
    { 'A', 'D', 'C', AM_ABX },
    { 'R', 'O', 'R', AM_ABX },
    { 'R', 'R', 'A', AM_ABX },

    { 'N', 'O', 'P', AM_IMM },
    { 'S', 'T', 'A', AM_INX },
    { 'N', 'O', 'P', AM_IMM },
    { 'S', 'A', 'X', AM_INX },
    { 'S', 'T', 'Y', AM_ZPG },
    { 'S', 'T', 'A', AM_ZPG },
    { 'S', 'T', 'X', AM_ZPG },
    { 'S', 'A', 'X', AM_ZPG },
    { 'D', 'E', 'Y', AM_IMP },
    { 'N', 'O', 'P', AM_IMM },
    { 'T', 'X', 'A', AM_IMP },
    { 'X', 'X', 'A', AM_IMM },
    { 'S', 'T', 'Y', AM_ABS },
    { 'S', 'T', 'A', AM_ABS },
    { 'S', 'T', 'X', AM_ABS },
    { 'S', 'A', 'X', AM_ABS },

    { 'B', 'C', 'C', AM_REL },
    { 'S', 'T', 'A', AM_INY },
    { 'S', 'T', 'P', AM_UNK },
    { 'A', 'H', 'X', AM_INY },
    { 'S', 'T', 'Y', AM_ZPX },
    { 'S', 'T', 'A', AM_ZPX },
    { 'S', 'T', 'X', AM_ZPY },
    { 'S', 'A', 'X', AM_ZPY },
    { 'T', 'Y', 'A', AM_IMP },
    { 'S', 'T', 'A', AM_ABY },
    { 'T', 'X', 'S', AM_IMP },
    { 'T', 'A', 'S', AM_ABY },
    { 'S', 'H', 'Y', AM_ABX },
    { 'S', 'T', 'A', AM_ABX },
    { 'S', 'H', 'X', AM_ABY },
    { 'A', 'H', 'X', AM_ABY },

    { 'L', 'D', 'Y', AM_IMM },
    { 'L', 'D', 'A', AM_INX },
    { 'L', 'D', 'X', AM_IMM },
    { 'L', 'A', 'X', AM_INX },
    { 'L', 'D', 'Y', AM_ZPG },
    { 'L', 'D', 'A', AM_ZPG },
    { 'L', 'D', 'X', AM_ZPG },
    { 'L', 'A', 'X', AM_ZPG },
    { 'T', 'A', 'Y', AM_IMP },
    { 'L', 'D', 'A', AM_IMM },
    { 'T', 'A', 'X', AM_IMP },
    { 'L', 'A', 'X', AM_IMM },
    { 'L', 'D', 'Y', AM_ABS },
    { 'L', 'D', 'A', AM_ABS },
    { 'L', 'D', 'X', AM_ABS },
    { 'L', 'A', 'X', AM_ABS },

    { 'B', 'C', 'S', AM_REL },
    { 'L', 'D', 'A', AM_INY },
    { 'S', 'T', 'P', AM_UNK },
    { 'L', 'A', 'X', AM_INY },
    { 'L', 'D', 'Y', AM_ZPX },
    { 'L', 'D', 'A', AM_ZPX },
    { 'L', 'D', 'X', AM_ZPY },
    { 'L', 'A', 'X', AM_ZPY },
    { 'C', 'L', 'V', AM_IMP },
    { 'L', 'D', 'A', AM_ABY },
    { 'T', 'S', 'X', AM_IMP },
    { 'L', 'A', 'S', AM_ABY },
    { 'L', 'D', 'Y', AM_ABX },
    { 'L', 'D', 'A', AM_ABX },
    { 'L', 'D', 'X', AM_ABY },
    { 'L', 'A', 'X', AM_ABY },

    { 'C', 'P', 'Y', AM_IMM },
    { 'C', 'M', 'P', AM_INX },
    { 'N', 'O', 'P', AM_IMM },
    { 'D', 'C', 'P', AM_INX },
    { 'C', 'P', 'Y', AM_ZPG },
    { 'C', 'M', 'P', AM_ZPG },
    { 'D', 'E', 'C', AM_ZPG },
    { 'D', 'C', 'P', AM_ZPG },
    { 'I', 'N', 'Y', AM_IMP },
    { 'C', 'M', 'P', AM_IMM },
    { 'D', 'E', 'X', AM_IMP },
    { 'A', 'X', 'S', AM_IMM },
    { 'C', 'P', 'Y', AM_ABS },
    { 'C', 'M', 'P', AM_ABS },
    { 'D', 'E', 'C', AM_ABS },
    { 'D', 'C', 'P', AM_ABS },

    { 'B', 'N', 'E', AM_REL },
    { 'C', 'M', 'P', AM_INY },
    { 'S', 'T', 'P', AM_UNK },
    { 'D', 'C', 'P', AM_INY },
    { 'N', 'O', 'P', AM_ZPX },
    { 'C', 'M', 'P', AM_ZPX },
    { 'D', 'E', 'C', AM_ZPX },
    { 'D', 'C', 'P', AM_ZPX },
    { 'C', 'L', 'D', AM_IMP },
    { 'C', 'M', 'P', AM_ABY },
    { 'N', 'O', 'P', AM_IMP },
    { 'D', 'C', 'P', AM_ABY },
    { 'N', 'O', 'P', AM_ABX },
    { 'C', 'M', 'P', AM_ABX },
    { 'D', 'E', 'C', AM_ABX },
    { 'D', 'C', 'P', AM_ABX },

    { 'C', 'P', 'X', AM_IMM },
    { 'S', 'B', 'C', AM_INX },
    { 'N', 'O', 'P', AM_IMM },
    { 'I', 'S', 'B', AM_INX },
    { 'C', 'P', 'X', AM_ZPG },
    { 'S', 'B', 'C', AM_ZPG },
    { 'I', 'N', 'C', AM_ZPG },
    { 'I', 'S', 'B', AM_ZPG },
    { 'I', 'N', 'X', AM_IMP },
    { 'S', 'B', 'C', AM_IMM },
    { 'N', 'O', 'P', AM_IMP },
    { 'S', 'B', 'C', AM_IMM },
    { 'C', 'P', 'X', AM_ABS },
    { 'S', 'B', 'C', AM_ABS },
    { 'I', 'N', 'C', AM_ABS },
    { 'I', 'S', 'B', AM_ABS },

    { 'B', 'E', 'Q', AM_REL },
    { 'S', 'B', 'C', AM_INY },
    { 'S', 'T', 'P', AM_UNK },
    { 'I', 'S', 'B', AM_INY },
    { 'N', 'O', 'P', AM_ZPX },
    { 'S', 'B', 'C', AM_ZPX },
    { 'I', 'N', 'C', AM_ZPX },
    { 'I', 'S', 'B', AM_ZPX },
    { 'S', 'E', 'D', AM_IMP },
    { 'S', 'B', 'C', AM_ABY },
    { 'N', 'O', 'P', AM_IMP },
    { 'I', 'S', 'B', AM_ABY },
    { 'N', 'O', 'P', AM_ABX },
    { 'S', 'B', 'C', AM_ABX },
    { 'I', 'N', 'C', AM_ABX },
    { 'I', 'S', 'B', AM_ABX },
};
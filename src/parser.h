#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_INSTR_COUNT 32768
#define BASE_ADDR 0x200

enum instr_type {
    INSTRT_DOUBLE,
    INSTRT_SINGLE,
    INSTRT_WITHOUT,
    INSTRT_BRANCH,
    INSTRT_DIRECTIVE
};

enum instr_double {
    INSTRD_MOV,
    INSTRD_MOVB,
    INSTRD_CMP,
    INSTRD_CMPB,
    INSTRD_BIT,
    INSTRD_BITB,
    INSTRD_BIC,
    INSTRD_BICB,
    INSTRD_BIS,
    INSTRD_BISB,
    INSTRD_ADD,
    INSTRD_SUB,
    INSTRD_JSR
};

enum instr_single {
    INSTRS_JMP,
    INSTRS_SWAB,
    INSTRS_CLR,
    INSTRS_CLRB,
    INSTRS_COM,
    INSTRS_COMB,
    INSTRS_INC,
    INSTRS_INCB,
    INSTRS_DEC,
    INSTRS_DECB,
    INSTRS_NEG,
    INSTRS_NEGB,
    INSTRS_ADC,
    INSTRS_ADCB,
    INSTRS_SBC,
    INSTRS_SBCB,
    INSTRS_TST,
    INSTRS_TSTB,
    INSTRS_ROR,
    INSTRS_RORB,
    INSTRS_ROL,
    INSTRS_ROLB,
    INSTRS_ASR,
    INSTRS_ASRB,
    INSTRS_ASL,
    INSTRS_ASLB,
    INSTRS_MTPS,
    INSTRS_MFPS,
    INSTRS_RTS
};

enum instr_without {
    INSTRW_CLC,
    INSTRW_CLV,
    INSTRW_CLZ,
    INSTRW_CLN,
    INSTRW_CCC,
    INSTRW_SEC,
    INSTRW_SEV,
    INSTRW_SEZ,
    INSTRW_SEN,
    INSTRW_SCC,
    INSTRW_HALT,
    INSTRW_WAIT,
    INSTRW_NOP
};

enum instr_branch {
    INSTRB_BR,
    INSTRB_BNE,
    INSTRB_BEQ,
    INSTRB_BGE,
    INSTRB_BLT,
    INSTRB_BGT,
    INSTRB_BLE,
    INSTRB_BPL,
    INSTRB_BMI,
    INSTRB_BHI,
    INSTRB_BLOS,
    INSTRB_BVC,
    INSTRB_BVS,
    INSTRB_BCC,
    INSTRB_BCS,
    INSTRB_SOB
};

enum directive {
    DIR_BYTE,
    DIR_WORD,
    DIR_BLKB,
    DIR_BLKW
};

enum addr_mode {
    AMOD_REG,
    AMOD_REG_DEF,
    AMOD_INC,
    AMOD_INC_DEF,
    AMOD_DEC,
    AMOD_DEC_DEF,
    AMOD_IND,
    AMOD_IND_DEF
};

enum operand_type {
    OPT_REG,
    OPT_IMM,
    OPT_MEM
};

struct operand {
    enum addr_mode mode;
    enum operand_type type;
    uint8_t regno;
    union {
        uint16_t mem_off;
        uint16_t imm;
    };
};

struct instr_entry {
    enum instr_type type;
    union {
        enum instr_double instrd;
        enum instr_single instrs;
        enum instr_without instrw;
        enum instr_branch instrb;
        enum directive dir;
    };
    struct operand op1;
    struct operand op2;
};

int read_file(const char *filename);
void parse_line(char *line);
char *parse_label(char *label);

#endif //PARSER_H
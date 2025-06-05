#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "label.h"
#include "diagnostic.h"
#include "func.h"

#define MAX_INSTR_COUNT 32768
#define BASE_ADDR 01000
#define MAX_BUFFER_SIZE 256

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
    INSTRB_BHIS,
    INSTRB_BLO,
    INSTRB_BVC,
    INSTRB_BVS,
    INSTRB_BCC,
    INSTRB_BCS
};

enum directive_type {
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

struct instr_info {
    enum instr_type type;
    union {
        enum instr_double instrd;
        enum instr_single instrs;
        enum instr_without instrw;
        enum instr_branch instrb;
        enum directive_type dir;
    };
    union {
        uint16_t opcode;
        char *ptr;
    };
    uint16_t value;
    char mnemonic[8];
    bool is_byte;
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
    struct instr_info instr;
    struct operand op1;
    struct operand op2;
};

struct realloc_table {
    struct operand *op;
    char label[MAX_LABEL_LENGTH + 1];
    size_t cur_addr;
};

extern struct diagnostic_queue *dq;
extern struct instr_entry entry[MAX_INSTR_COUNT];
extern uint32_t instrno;

int read_file(const char *filename);
char *read_line(FILE *file);
void firt_pass(FILE *file);
void second_pass(FILE *file);
void parse_line(char *line);
void parse_label(char *label);
char *skip_label(char *label);
void parse_instr(char *instr);
bool mnemonic_exists(struct instr_info *instruction, const char *mnemonic);
int parse_operands(char *operands);
int parse_operand(char *op, struct operand *out);
int parse_immediate(char *imm, struct operand *out);
int parse_memory(char *mem, struct operand *out);
int parse_register(char *reg, struct operand *out);
int parse_indexed(char *text, struct operand *out);
int parse_branch(char *op, struct operand *out);
void resolve_mem_off(struct operand *op, char *name);
bool is_immediate(char *imm);
bool is_rn(char *reg, int offset);
bool is_register(char *reg);
bool is_register_def(char *reg);
bool is_autoinc(char *reg);
bool is_autodec(char *reg);
void parse_derective(char *dir);
int parse_dirops(char *operands);
int parse_string(char *string, uint16_t *op_count, char **ptr);
int parse_value(char *imm, int64_t *value);
int parse_byte_and_word(char *str, uint16_t *op_count, char **ptr);
int parse_block(char *str, int64_t *value);
char *find_comma_outside(char *str);

#endif //PARSER_H
#include "parser.h"
#include "label.h"
#include "diagnostic.h"
#include "func.h"

struct diagnostic_queue *dq;
struct label_table ltab = {0};
struct instr_entry entry[MAX_INSTR_COUNT];
uint32_t instrno;
uint32_t lineno;
size_t addr = BASE_ADDR;
FILE *input;

struct instr_info instructions[] = {
    { INSTRT_DOUBLE, INSTRD_MOV, "mov", false },
    { INSTRT_DOUBLE, INSTRD_MOVB, "movb", true },
    { INSTRT_DOUBLE, INSTRD_CMP, "cmp", false },
    { INSTRT_DOUBLE, INSTRD_CMPB, "cmpb", true },
    { INSTRT_DOUBLE, INSTRD_BIT, "bit", false },
    { INSTRT_DOUBLE, INSTRD_BITB, "bitb", true },
    { INSTRT_DOUBLE, INSTRD_BIC, "bic", false },
    { INSTRT_DOUBLE, INSTRD_BICB, "bicb", true },
    { INSTRT_DOUBLE, INSTRD_BIS, "bis", false },
    { INSTRT_DOUBLE, INSTRD_BISB, "bisb", true },
    { INSTRT_DOUBLE, INSTRD_ADD, "add", false },
    { INSTRT_DOUBLE, INSTRD_SUB, "sub", false },
    { INSTRT_DOUBLE, INSTRD_JSR, "jsr", false },
    { INSTRT_SINGLE, INSTRS_JMP, "jmp" },
    { INSTRT_SINGLE, INSTRS_SWAB, "swab" },
    { INSTRT_SINGLE, INSTRS_CLR, "clr" },
    { INSTRT_SINGLE, INSTRS_CLRB, "clrb" },
    { INSTRT_SINGLE, INSTRS_COM, "com" },
    { INSTRT_SINGLE, INSTRS_COMB, "comb" },
    { INSTRT_SINGLE, INSTRS_INC, "inc" },
    { INSTRT_SINGLE, INSTRS_INCB, "incb" },
    { INSTRT_SINGLE, INSTRS_DEC, "dec" },
    { INSTRT_SINGLE, INSTRS_DECB, "decb" },
    { INSTRT_SINGLE, INSTRS_NEG, "neg" },
    { INSTRT_SINGLE, INSTRS_NEGB, "negb" },
    { INSTRT_SINGLE, INSTRS_ADC, "adc" },
    { INSTRT_SINGLE, INSTRS_ADCB, "adcb" },
    { INSTRT_SINGLE, INSTRS_SBC, "sbc" },
    { INSTRT_SINGLE, INSTRS_SBCB, "sbcb" },
    { INSTRT_SINGLE, INSTRS_TST, "tst" },
    { INSTRT_SINGLE, INSTRS_TSTB, "tstb" },
    { INSTRT_SINGLE, INSTRS_ROR, "ror" },
    { INSTRT_SINGLE, INSTRS_RORB, "rorb" },
    { INSTRT_SINGLE, INSTRS_ROL, "rol" },
    { INSTRT_SINGLE, INSTRS_ROLB, "rolb" },
    { INSTRT_SINGLE, INSTRS_ASR, "asr" },
    { INSTRT_SINGLE, INSTRS_ASRB, "asrb" },
    { INSTRT_SINGLE, INSTRS_ASL, "asl" },
    { INSTRT_SINGLE, INSTRS_ASLB, "aslb" },
    { INSTRT_SINGLE, INSTRS_MTPS, "mtps" },
    { INSTRT_SINGLE, INSTRS_MFPS, "mfps" },
    { INSTRT_SINGLE, INSTRS_RTS, "rts" },
    { INSTRT_WITHOUT, INSTRW_CLC, "clc" },
    { INSTRT_WITHOUT, INSTRW_CLV, "clv" },
    { INSTRT_WITHOUT, INSTRW_CLZ, "clz" },
    { INSTRT_WITHOUT, INSTRW_CLN, "cln" },
    { INSTRT_WITHOUT, INSTRW_CCC, "ccc" },
    { INSTRT_WITHOUT, INSTRW_SEC, "sec" },
    { INSTRT_WITHOUT, INSTRW_SEV, "sev" },
    { INSTRT_WITHOUT, INSTRW_SEZ, "sez" },
    { INSTRT_WITHOUT, INSTRW_SEN, "sen" },
    { INSTRT_WITHOUT, INSTRW_SCC, "SCC" },
    { INSTRT_WITHOUT, INSTRW_HALT, "halt" },
    { INSTRT_WITHOUT, INSTRW_WAIT, "wait" },
    { INSTRT_WITHOUT, INSTRW_NOP, "nop" },
    { INSTRT_BRANCH, INSTRB_BR, "br" },
    { INSTRT_BRANCH, INSTRB_BNE, "bne" },
    { INSTRT_BRANCH, INSTRB_BEQ, "beq" },
    { INSTRT_BRANCH, INSTRB_BGE, "bge" },
    { INSTRT_BRANCH, INSTRB_BLT, "blt" },
    { INSTRT_BRANCH, INSTRB_BGT, "bgt" },
    { INSTRT_BRANCH, INSTRB_BLE, "ble" },
    { INSTRT_BRANCH, INSTRB_BPL, "bpl" },
    { INSTRT_BRANCH, INSTRB_BMI, "bmi" },
    { INSTRT_BRANCH, INSTRB_BHI, "bhi" },
    { INSTRT_BRANCH, INSTRB_BLOS, "blos" },
    { INSTRT_BRANCH, INSTRB_BHIS, "bhis" },
    { INSTRT_BRANCH, INSTRB_BLO, "blo" },
    { INSTRT_BRANCH, INSTRB_BVC, "bvc" },
    { INSTRT_BRANCH, INSTRB_BVS, "bvs" },
    { INSTRT_BRANCH, INSTRB_BCC, "bcc" },
    { INSTRT_BRANCH, INSTRB_BCS, "bcs" },
    { INSTRT_BRANCH, INSTRB_SOB, "sob" },
    { INSTRT_DIRECTIVE, DIR_BYTE, "byte" },
    { INSTRT_DIRECTIVE, DIR_WORD, "word" },
    { INSTRT_DIRECTIVE, DIR_BLKB, "blkb" },
    { INSTRT_DIRECTIVE, DIR_BLKW, "blkw" }
};

int read_file(const char *filename) {
    input = fopen(filename, "r");
    if (!input) return -1;

    char buffer[MAX_BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), input)) {
        char *pos = strchr(buffer, '\n');
        if (pos) {
            *pos = '\0';
        } else {
            int c;
            while ((c = fgetc(input)) != '\n' && c != EOF);
        }

        lineno++;
        replace_char(buffer, '\t', ' ');
        char *line = skip_spaces(buffer);
        if (line[0] == '\0' || line[0] == ';') continue;

        parse_line(line);
    }

    return 0;
}

void parse_line(char *line) {
    char *after_label = parse_label(line);
    if (after_label) {
        if (*after_label == '\0' || *after_label == ';') return;
        else line = after_label;
    }

    parse_instr(line);
}

char *parse_label(char *label) {
    char *after_label = NULL;
    char *colon = strchr(label, ':');
    if (colon) {
        if (*(colon + 1) != ' ' && *(colon + 1) != '\0') {
            diagnostic_add(dq, DIAGL_ERROR, lineno, "invalid label definition");
        } else {
            *colon = '\0';
            str_to_lower(label);
            if (label[0] == '\0') {
                diagnostic_add(dq, DIAGL_ERROR, lineno, "label name expected");
            } else if (strlen(label) > MAX_LABEL_LENGTH) {
                diagnostic_add(dq, DIAGL_ERROR, lineno, "max 16 chars label length");
            } else if (!is_valid_name(label)) {
                diagnostic_add(dq, DIAGL_ERROR, lineno, "invalid label name");
            } else if (label_exists(&ltab, label)) {
                diagnostic_add(dq, DIAGL_ERROR, lineno, "label '%s' redefined", label);
            } else if (label_add(&ltab, label, addr) == -1) {
                diagnostic_add(dq, DIAGL_ERROR, lineno, "max 512 label count");
            }
        }

        after_label = skip_spaces(colon + 1);
    }
 

    return after_label;
}

void parse_instr(char *instr) {
    char mnemonic[8];
    sscanf(instr, "%7s", mnemonic);

    size_t len = strlen(mnemonic);
    if (instr[len] != ' ' && instr[len] != '\0') {
        diagnostic_add(dq, DIAGL_ERROR, lineno, "instruction expected");
        return;
    }

    str_to_lower(mnemonic);
    if (!mnemonic_exists(&entry[instrno].instr, mnemonic)) {
        diagnostic_add(dq, DIAGL_ERROR, lineno, "instruction expected");
        return;
    }

    instr += len;
    char *after_mnemonic = skip_spaces(instr);
    parse_operands(after_mnemonic);
}

int mnemonic_exists(struct instr_info *instruction, const char *mnemonic) {
    size_t len = strlen(mnemonic);
    int flag = 0;

    for (int i = 0; i < INSTRUCTIONS_SIZE; i++) {
        if (len != strlen(instructions[i].mnemonic)) continue;
        if (memcmp(mnemonic, instructions[i].mnemonic, len) == 0) {
            *instruction = instructions[i];
            flag = 1;
            break;
        }
    }

    return flag;
}

void parse_operands(char *operands) {
    uint8_t op_count;
    char *op1, *op2;
    char *comma = strchr(operands, ',');
    if (comma) {
        *comma = '\0';
        op1 = operands;
        replace_char(op1, ' ', '\0');
        char *after_comma = skip_spaces(comma + 1);
        op2 = after_comma;
        while (*after_comma != ' ' && *after_comma != '\0') after_comma++;
        *after_comma = '\0';
        op_count = 2;
    } else if (*operands != '\0' && *operands != ';') {
        char *space = strchr(operands, ' ');
        if (space) {
            *space = '\0';
            char *after_space = skip_spaces(space + 1);
            if (*after_space != '\0' && *after_space != ';') {
                diagnostic_add(dq, DIAGL_ERROR, lineno, "comma expected");
                return;
            }
        }
        op1 = operands;
        op_count = 1;
    } else {
        op_count = 0;
    }

    addr += 2;
    enum instr_type type = entry[instrno].instr.type;
    if (type == INSTRT_DOUBLE && op_count != 2) {
        diagnostic_add(dq, DIAGL_ERROR, lineno, "2 operands expected");
        return;
    } else if ((type == INSTRT_SINGLE || type == INSTRT_BRANCH) && op_count != 1) {
        diagnostic_add(dq, DIAGL_ERROR, lineno, "1 operand expected");
        return;
    } else if (type == INSTRT_WITHOUT && op_count != 0) {
        diagnostic_add(dq, DIAGL_ERROR, lineno, "too many operands");
        return;
    }
}

bool parse_operand(char *op, struct operand *out) {
    char *op_dup[32];
    snprintf(op_dup, sizeof(op_dup), "%s", op);
    str_to_lower(op);

    if (starts_with(op, "@#")) {
        out->mode = AMOD_INC_DEF;
        out->regno = 7;
        if (is_immediate(op_dup + 2)) return parse_immediate(op_dup + 2, out);
        else return parse_memory(op + 2, out);
    } 
    if (*op == '#') {
        out->mode = AMOD_INC;
        out->regno = 7;
        if (is_immediate(op_dup + 1)) return parse_immediate(op_dup + 1, out);
        else return parse_memory(op + 1, out);
    }
    if (*op == '@') {
        out->mode = AMOD_IND_DEF;
        if (is_autoinc(op + 1)) return parse_register(op + 2, out);
        if (is_autodec(op + 1)) return parse_register(op + 3, out);
        return parse_indexed(op + 1, out);
    }
}

bool parse_immediate(char *imm, struct operand *out) {
    if (*imm == '"') {
        imm++;
        char *quot_mark = strchr(imm, '"');
        if (!quot_mark) {
            diagnostic_add(dq, DIAGL_ERROR, lineno, "invalid operand");
            return false;
        }

        *quot_mark = '\0';
        if (*(quot_mark + 1) != '\0') {
            diagnostic_add(dq, DIAGL_ERROR, lineno, "invalid operand");
            return false;
        } else if (strlen(imm) != 1) {
            diagnostic_add(dq, DIAGL_ERROR, lineno, "invalid operand");
            return false;
        }

        out->imm = (uint16_t)*imm;
    } else if ((*imm >= '0' && *imm <= '9') || *imm == '-') {
        str_to_lower(imm);
        int base = 8;
        size_t len = strlen(imm);

        switch (imm[len - 1]) {
            case 'd': base = 10; imm[len - 1] = '\0'; break;
            case 'h': base = 16; imm[len - 1] = '\0'; break;
            case 'b': base = 2; imm[len - 1] = '\0'; break;
        }

        char *end;
        int64_t value = strtoll(imm, &end, base);
        if (*end != '\0') {
            diagnostic_add(dq, DIAGL_ERROR, lineno, "invalid operand");
            return false;
        }
            
        if (entry[instrno].instr.is_byte && (value < INT8_MIN || value > INT8_MAX)) {
            diagnostic_add(dq, DIAGL_WARNING, lineno, "overlow in immediate value");
            value &= 0xFF;
        } else if (!entry[instrno].instr.is_byte && (value < INT16_MIN || value > INT16_MAX)) {
            diagnostic_add(dq, DIAGL_WARNING, lineno, "overlow in immediate value");
            value &= 0xFFFF;
        }

        out->imm = (uint16_t)value;
    }

    out->type = OPT_IMM;
    return true;
}

bool parse_memory(char *mem, struct operand *out) {
    if (!is_valid_name(mem)) {
        diagnostic_add(dq, DIAGL_ERROR, lineno, "invalid operand");
        return false;
    }

    if (label_exists(&ltab, mem)) {
        size_t label_addr = get_label_addr(&ltab, mem);
        out->mem_off = (uint16_t)label_addr;
        out->type = OPT_MEM;
        return true;
    } else {
        diagnostic_add(dq, DIAGL_ERROR, lineno, "invalid operand");
        return false;
    }
}

bool parse_register(char *reg, struct operand *out) {
    if (starts_with(reg, "sp")) out->regno = 6;
    else if (starts_with(reg, "pc")) out->regno = 7;
    else out->regno = *(reg + 1) - '0';
    out->type = OPT_REG;
    return true;
}

bool parse_indexed(char *text, struct operand *out) {
    if (*text < '0' || *text > '9') {
        bool flag = parse_memory(text, out);
        if (flag) out->mem_off -= addr + 2;
        return flag;
    } else {
        char *end;
        int64_t value = strtoll(text, &end, 8);
        if (*end == '\0' || !is_register_def(*end)) {
            diagnostic_add(dq, DIAGL_ERROR, lineno, "invalid operand");
            return false;
        }
        out->mem_off = (uint16_t)value;
        parse_register(end + 1, out);
        return true;
    }
}

bool is_immediate(char *imm) {
    return *imm == '"' || *imm == '-' || (*imm >= 0 && *imm <= 9);
}

bool is_register(char *reg) {
    return (reg[0] == 'r' && reg[1] >= '0' && reg[1] <= '7' && reg[2] == '\0') ||
           (strcmp(reg, "sp") == 0) || (strcmp(reg, "pc") == 0);
}

bool is_register_def(char *reg) {
    return (reg[0] == '(' && reg[1] == 'r' && reg[2] >= '0' && reg[2] <= '7' &&
           reg[3] == ')' && reg[4] == '\0') || (strcmp(reg, "(sp)") == 0) ||
           (strcmp(reg, "(pc)") == 0);
}

bool is_autoinc(char *reg) {
    return (reg[0] == '(' && reg[1] == 'r' && reg[2] >= '0' && reg[2] <= '7' &&
           reg[3] == ')' && reg[4] == '+' && reg[5] == '\0') || (strcmp(reg, "(sp)+") == 0) ||
           (strcmp(reg, "(pc)+") == 0);
}

bool is_autodec(char *reg) {
    return (reg[0] == '-' && reg[1] == '(' && reg[2] == 'r' && reg[3] >= '0' && reg[3] <= '7' &&
           reg[4] == ')' && reg[5] == '\0') || (strcmp(reg, "-(sp)") == 0) ||
           (strcmp(reg, "-(pc)") == 0);
}

bool is_indexed(char *reg) {
    char *end;
    int64_t value = strtoll(reg, &end, 8);
    if (*end == '\0') return false;

    reg = end;
    return is_register_def(reg);
}
#include "parser.h"

#define INSTRUCTIONS_SIZE (sizeof(instructions) / sizeof(instructions[0]))

struct diagnostic_queue *dq;
struct label_table ltab = {0};
struct instr_entry entry[MAX_INSTR_COUNT];
struct realloc_table rtab[MAX_INSTR_COUNT >> 2];
uint32_t instrno;
uint16_t rtabno;
uint32_t lineno;
size_t addr = BASE_ADDR;
char buffer[MAX_BUFFER_SIZE];
bool is_inc_mode = false;

struct instr_info instructions[] = {
    { .type = INSTRT_DOUBLE, .info = INSTR_NONE, .mnemonic = "mov", .opcode = 001, .is_byte = false },
    { .type = INSTRT_DOUBLE, .info = INSTR_NONE, .mnemonic = "movb", .opcode = 011, .is_byte = true },
    { .type = INSTRT_DOUBLE, .info = INSTR_CMP, .mnemonic = "cmp", .opcode = 002, .is_byte = false },
    { .type = INSTRT_DOUBLE, .info = INSTR_CMPB, .mnemonic = "cmpb", .opcode = 012, .is_byte = true },
    { .type = INSTRT_DOUBLE, .info = INSTR_BIT, .mnemonic = "bit", .opcode = 003, .is_byte = false },
    { .type = INSTRT_DOUBLE, .info = INSTR_BITB, .mnemonic = "bitb", .opcode = 013, .is_byte = true },
    { .type = INSTRT_DOUBLE, .info = INSTR_NONE, .mnemonic = "bic", .opcode = 004, .is_byte = false },
    { .type = INSTRT_DOUBLE, .info = INSTR_NONE, .mnemonic = "bicb", .opcode = 014, .is_byte = true },
    { .type = INSTRT_DOUBLE, .info = INSTR_NONE, .mnemonic = "bis", .opcode = 005, .is_byte = false },
    { .type = INSTRT_DOUBLE, .info = INSTR_NONE, .mnemonic = "bisb", .opcode = 015, .is_byte = true },
    { .type = INSTRT_DOUBLE, .info = INSTR_NONE, .mnemonic = "add", .opcode = 006, .is_byte = false },
    { .type = INSTRT_DOUBLE, .info = INSTR_NONE, .mnemonic = "sub", .opcode = 016, .is_byte = false },
    { .type = INSTRT_DOUBLE, .info = INSTR_JSR, .mnemonic = "jsr", .opcode = 0004, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "jmp", .opcode = 00001, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "swab", .opcode = 00003, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "clr", .opcode = 00050, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "clrb", .opcode = 01050, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "com", .opcode = 00051, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "comb", .opcode = 01051, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "inc", .opcode = 00052, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "incb", .opcode = 01052, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "dec", .opcode = 00053, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "decb", .opcode = 01053, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "neg", .opcode = 00054, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "negb", .opcode = 01054, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "adc", .opcode = 00055, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "adcb", .opcode = 01055, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "sbc", .opcode = 00056, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "sbcb", .opcode = 01056, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "tst", .opcode = 00057, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "tstb", .opcode = 01057, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "ror", .opcode = 00060, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "rorb", .opcode = 01060, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "rol", .opcode = 00061, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "rolb", .opcode = 01061, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "asr", .opcode = 00062, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "asrb", .opcode = 01062, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "asl", .opcode = 00063, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "aslb", .opcode = 01063, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "mtps", .opcode = 01064, .is_byte = false},
    { .type = INSTRT_SINGLE, .info = INSTR_NONE, .mnemonic = "mfps", .opcode = 01067, .is_byte = false },
    { .type = INSTRT_SINGLE, .info = INSTR_RTS, .mnemonic = "rts", .opcode = 000020, .is_byte = false },
    { .type = INSTRT_WITHOUT, .info = INSTR_NONE, .mnemonic = "clc", .opcode = 0000241, .is_byte = false },
    { .type = INSTRT_WITHOUT, .info = INSTR_NONE, .mnemonic = "clv", .opcode = 0000242, .is_byte = false },
    { .type = INSTRT_WITHOUT, .info = INSTR_NONE, .mnemonic = "clz", .opcode = 0000244, .is_byte = false },
    { .type = INSTRT_WITHOUT, .info = INSTR_NONE, .mnemonic = "cln", .opcode = 0000250, .is_byte = false },
    { .type = INSTRT_WITHOUT, .info = INSTR_NONE, .mnemonic = "ccc", .opcode = 0000257, .is_byte = false },
    { .type = INSTRT_WITHOUT, .info = INSTR_NONE, .mnemonic = "sec", .opcode = 0000261, .is_byte = false },
    { .type = INSTRT_WITHOUT, .info = INSTR_NONE, .mnemonic = "sev", .opcode = 0000262, .is_byte = false },
    { .type = INSTRT_WITHOUT, .info = INSTR_NONE, .mnemonic = "sez", .opcode = 0000264, .is_byte = false },
    { .type = INSTRT_WITHOUT, .info = INSTR_NONE, .mnemonic = "sen", .opcode = 0000270, .is_byte = false },
    { .type = INSTRT_WITHOUT, .info = INSTR_NONE, .mnemonic = "scc", .opcode = 0000277, .is_byte = false },
    { .type = INSTRT_WITHOUT, .info = INSTR_NONE, .mnemonic = "halt", .opcode = 0000000, .is_byte = false },
    { .type = INSTRT_WITHOUT, .info = INSTR_NONE, .mnemonic = "wait", .opcode = 0000001, .is_byte = false },
    { .type = INSTRT_WITHOUT, .info = INSTR_NONE, .mnemonic = "nop", .opcode = 0000240, .is_byte = false },
    { .type = INSTRT_BRANCH, .info = INSTR_NONE, .mnemonic = "br", .opcode = 0001, .is_byte = false },
    { .type = INSTRT_BRANCH, .info = INSTR_NONE, .mnemonic = "bne", .opcode = 0002, .is_byte = false },
    { .type = INSTRT_BRANCH, .info = INSTR_NONE, .mnemonic = "beq", .opcode = 0003, .is_byte = false },
    { .type = INSTRT_BRANCH, .info = INSTR_NONE, .mnemonic = "bge", .opcode = 0004, .is_byte = false },
    { .type = INSTRT_BRANCH, .info = INSTR_NONE, .mnemonic = "blt", .opcode = 0005, .is_byte = false },
    { .type = INSTRT_BRANCH, .info = INSTR_NONE, .mnemonic = "bgt", .opcode = 0006, .is_byte = false },
    { .type = INSTRT_BRANCH, .info = INSTR_NONE, .mnemonic = "ble", .opcode = 0007, .is_byte = false },
    { .type = INSTRT_BRANCH, .info = INSTR_NONE, .mnemonic = "bpl", .opcode = 0200, .is_byte = false },
    { .type = INSTRT_BRANCH, .info = INSTR_NONE, .mnemonic = "bmi", .opcode = 0201, .is_byte = false },
    { .type = INSTRT_BRANCH, .info = INSTR_NONE, .mnemonic = "bhi", .opcode = 0202, .is_byte = false },
    { .type = INSTRT_BRANCH, .info = INSTR_NONE, .mnemonic = "blos", .opcode = 0203, .is_byte = false },
    { .type = INSTRT_BRANCH, .info = INSTR_NONE, .mnemonic = "bhis", .opcode = 0206, .is_byte = false },
    { .type = INSTRT_BRANCH, .info = INSTR_NONE, .mnemonic = "blo", .opcode = 0207, .is_byte = false },
    { .type = INSTRT_BRANCH, .info = INSTR_NONE, .mnemonic = "bvc", .opcode = 0204, .is_byte = false },
    { .type = INSTRT_BRANCH, .info = INSTR_NONE, .mnemonic = "bvs", .opcode = 0205, .is_byte = false },
    { .type = INSTRT_BRANCH, .info = INSTR_NONE, .mnemonic = "bcc", .opcode = 0206, .is_byte = false },
    { .type = INSTRT_BRANCH, .info = INSTR_NONE, .mnemonic = "bcs", .opcode = 0207, .is_byte = false },
    { .type = INSTRT_DIRECTIVE, .dir = DIR_BYTE, .mnemonic = ".byte", .ptr = NULL, .value = 0, .is_byte = true },
    { .type = INSTRT_DIRECTIVE, .dir = DIR_WORD, .mnemonic = ".word", .ptr = NULL, .value = 0, .is_byte = false },
    { .type = INSTRT_DIRECTIVE, .dir = DIR_BLKB, .mnemonic = ".blkb", .ptr = NULL, .value = 0, .is_byte = false },
    { .type = INSTRT_DIRECTIVE, .dir = DIR_BLKW, .mnemonic = ".blkw", .ptr = NULL, .value = 0, .is_byte = false }
};

int read_file(const char *filename) {
    FILE *input = fopen(filename, "r");
    if (!input) return -1;

    firt_pass(input);
    
    if (has_errors(dq)) {
        diagnostic_print(dq);
        diagnostic_free(dq);
        fclose(input);
        exit(EXIT_FAILURE);
    }

    lineno = 0;
    rewind(input);
    second_pass(input);

    if (has_errors(dq)) {
        diagnostic_print(dq);
        diagnostic_free(dq);
        fclose(input);
        for (uint32_t i = 0; i < instrno; i++) {
            if (entry[i].instr.type == INSTRT_DIRECTIVE) {
                free(entry[i].instr.ptr);
            }
        }
        exit(EXIT_FAILURE);
    }

    diagnostic_print(dq);
    diagnostic_free(dq);

    fclose(input);

    for (uint32_t i = 0; i < rtabno; i++) {
        addr = rtab[i].cur_addr;
        resolve_mem_off(rtab[i].op, rtab[i].label);
    }

    return 0;
}

char *read_line(FILE *file) {
    if (!fgets(buffer, sizeof(buffer), file)) return NULL;

    char *pos = strchr(buffer, '\n');
    if (pos) {
        *pos = '\0';
    } else {
        int c;
        while ((c = fgetc(file)) != '\n' && c != EOF);
    }

    lineno++;
    replace_char(buffer, '\t', ' ');
    replace_char(buffer, ';', '\0');
    char *line = skip_spaces(buffer);
    
    return line;
}

void firt_pass(FILE *file) {
    char *line;

    while ((line = read_line(file))) {
        if (line[0] == '\0') continue;
        parse_label(line);
    }
}

void parse_label(char *label) {
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
            } else if (label_add(&ltab, label) == -1) {
                diagnostic_add(dq, DIAGL_ERROR, lineno, "max 512 label count");
            }
        }
    }
}

void second_pass(FILE *file){
    char *line;

    while ((line = read_line(file))) {
        if (line[0] == '\0') continue;
        parse_line(line);
    }
}

void parse_line(char *line) {
    char *after_label = skip_label(line);
    if (after_label) {
        if (*after_label == '\0') return;
        else line = after_label;
    }

    if (line[0] == '.') parse_derective(line);
    else parse_instr(line);
}

char *skip_label(char *label) {
    char *after_label = NULL;
    char *colon = strchr(label, ':');

    if (colon) {
        *colon = '\0';
        str_to_lower(label);
        set_label_addr(&ltab, label, addr);
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

    addr += 2;
    instr += len;
    char *after_mnemonic = skip_spaces(instr);
    if (parse_operands(after_mnemonic) == -1) {
        diagnostic_add(dq, DIAGL_ERROR, lineno, "invalid operand");
    }
}

bool mnemonic_exists(struct instr_info *instruction, const char *mnemonic) {
    size_t len = strlen(mnemonic);

    for (uint64_t i = 0; i < INSTRUCTIONS_SIZE; i++) {
        if (len != strlen(instructions[i].mnemonic)) continue;
        if (memcmp(mnemonic, instructions[i].mnemonic, len) == 0) {
            *instruction = instructions[i];
            return true;
        }
    }

    return false;
}

int parse_operands(char *operands) {
    uint8_t op_count;
    char *op1, *op2;
    char *comma = find_comma_outside(operands);
    if (comma) {
        *comma = '\0';
        op1 = operands;
        char *after_op1 = find_space_outside(op1);
        if (after_op1) {
            *after_op1 = '\0';
            after_op1 = skip_spaces(after_op1 + 1);
            if (*after_op1 != '\0') return -1;
        }

        char *after_comma = skip_spaces(comma + 1);
        op2 = after_comma;
        while (*after_comma != ' ' && *after_comma != '\0') after_comma++;
        *after_comma = '\0';
        op_count = 2;
    } else if (*operands != '\0') {
        char *space = strchr(operands, ' ');
        if (space) {
            *space = '\0';
            char *after_space = skip_spaces(space + 1);
            if (*after_space != '\0') {
                diagnostic_add(dq, DIAGL_ERROR, lineno, "comma expected");
                return -2;
            }
        }
        op1 = operands;
        op_count = 1;
    } else {
        op_count = 0;
    }

    enum instr_type type = entry[instrno].instr.type;
    if (type == INSTRT_DOUBLE && op_count != 2) {
        diagnostic_add(dq, DIAGL_ERROR, lineno, "2 operands expected");
        return -2;
    } else if ((type == INSTRT_SINGLE || type == INSTRT_BRANCH) && op_count != 1) {
        diagnostic_add(dq, DIAGL_ERROR, lineno, "1 operand expected");
        return -2;
    } else if (type == INSTRT_WITHOUT && op_count != 0) {
        diagnostic_add(dq, DIAGL_ERROR, lineno, "too many operands");
        return -2;
    }

    struct instr_entry *ent = &entry[instrno];
    switch (type) {
        case INSTRT_DOUBLE:
            if (parse_operand(op1, &ent->op1) == -1) return -1;
            if (ent->instr.info == INSTR_JSR && ent->op1.mode != AMOD_REG) {
                return -1;
            }
            if (parse_operand(op2, &ent->op2) == -1) return -1;
            if (ent->instr.info != INSTR_CMP && ent->instr.info != INSTR_CMPB &&
                ent->instr.info != INSTR_BIT && ent->instr.info != INSTR_BIT &&
                (ent->op2.type == OPT_IMM || ent->op2.type == OPT_MEM) && 
                ent->op2.mode == AMOD_INC) return -1;
            break;
        case INSTRT_SINGLE:
            if (parse_operand(op1, &ent->op1) == -1) return -1;
            if ((ent->op1.type == OPT_IMM || ent->op1.type == OPT_MEM) &&
                ent->op1.mode == AMOD_INC) return -1;
            if (ent->instr.info == INSTR_RTS && ent->op1.mode != AMOD_REG) {
                return -1;
            }
            break;
        case INSTRT_BRANCH:
            if (parse_branch(op1, &ent->op1) == -1) return -1;
            break;
        default: break;
    }

    instrno++;
    return 0;
}

int parse_operand(char *op, struct operand *out) {
    char op_dup[32];
    snprintf(op_dup, sizeof(op_dup), "%s", op);
    str_to_lower(op);

    if (starts_with(op, "@#")) {
        out->mode = AMOD_INC_DEF;
        out->regno = 7;
        addr += 2;
        is_inc_mode = false;
        if (is_immediate(op_dup + 2)) return parse_immediate(op_dup + 2, out);
        return parse_memory(op + 2, out);
    } 
    if (*op == '#') {
        out->mode = AMOD_INC;
        out->regno = 7;
        addr += 2;
        is_inc_mode = true;
        if (is_immediate(op_dup + 1)) return parse_immediate(op_dup + 1, out);
        return parse_memory(op + 1, out);
    }
    if (*op == '@') {
        if (is_autoinc(op + 1)) {
            out->mode = AMOD_INC_DEF;
            return parse_register(op + 2, out);
        }
        if (is_autodec(op + 1)) {
            out->mode = AMOD_DEC_DEF;
            return parse_register(op + 3, out);
        }
        out->mode = AMOD_IND_DEF;
        addr += 2;
        return parse_indexed(op + 1, out);
    }
    if (is_register(op)) {
        out->mode = AMOD_REG;
        return parse_register(op, out);
    }
    if (is_register_def(op)) {
        out->mode = AMOD_REG_DEF;
        return parse_register(op + 1, out);
    }
    if (is_autoinc(op)) {
        out->mode = AMOD_INC;
        return parse_register(op + 1, out);
    }
    if (is_autodec(op)) {
        out->mode = AMOD_DEC;
        return parse_register(op + 2, out);
    }
    
    out->mode = AMOD_IND;
    addr += 2;
    return parse_indexed(op, out);
}

int parse_immediate(char *imm, struct operand *out) {
    if (*imm == '\'') {
        imm++;
        char *quot_mark = strchr(imm, '\'');
        if (!quot_mark) return -1;

        *quot_mark = '\0';
        if (*(quot_mark + 1) != '\0') return -1;
        if (strlen(imm) != 1) return -1;

        out->imm = (uint16_t)(unsigned char)*imm;
    } else {
        int64_t value;
        if (parse_value(imm, &value) == -1) return -1;
        out->imm = (uint16_t)value;
    }

    out->type = OPT_IMM;
    return 0;
}

int parse_memory(char *mem, struct operand *out) {
    if (!is_valid_name(mem)) return -1;

    if (label_exists(&ltab, mem)) {
        size_t label_addr = get_label_addr(&ltab, mem);
        if (label_addr == 0) {
            rtab[rtabno].op = out;
            rtab[rtabno].cur_addr = addr;
            snprintf(rtab[rtabno].label, MAX_LABEL_LENGTH + 1, "%s", mem);
            rtabno++;
        }

        out->mem_off = (uint16_t)label_addr;
        out->type = OPT_MEM;
        return 0;
    }

    return -1;
}

int parse_register(char *reg, struct operand *out) {
    if (starts_with(reg, "sp")) out->regno = 6;
    else if (starts_with(reg, "pc")) out->regno = 7;
    else out->regno = (unsigned char)*(reg + 1) - '0';
    out->type = OPT_REG;
    return 0;
}

int parse_indexed(char *text, struct operand *out) {
    int res = 0;

    if (is_immediate(text)) {
        char *end;
        int64_t value = strtoll(text, &end, 8);
        if (*end == '\0' || !is_register_def(end)) return -1;
        out->mem_off = (uint16_t)value;
        parse_register(end + 1, out);
    } else {
        out->regno = 7;
        res = parse_memory(text, out);
        if (res == 0) out->mem_off -= addr;
    }

    return res;
}

int parse_branch(char *op, struct operand *out) {
    if (is_immediate(op)) {
        char *end;
        int64_t value = strtoll(op, &end, 8);
        if (*end != '\0') return -1;
        if (value < INT8_MIN || value > INT8_MAX) {
            diagnostic_add(dq, DIAGL_ERROR, lineno, "too long branch, use jmp instead");
            return -2;
        }
        out->mem_off = (uint16_t)value;
        return 0;
    } else {
        int res = parse_memory(op, out);
        if (res == 0 && out->mem_off) {
            int16_t offset = (int16_t)out->mem_off;
            offset -= addr;
            offset /= 2;
            if (offset < INT8_MIN || offset > INT8_MAX) {
                diagnostic_add(dq, DIAGL_ERROR, lineno, "too long branch, use jmp instead");
                return -2;
            }
            out->mem_off = (uint16_t)offset;
            out->mode = AMOD_REG;
        }
        return res;
    }
}

void resolve_mem_off(struct operand *op, char *name) {
    switch (op->mode) {
        case AMOD_INC:
        case AMOD_INC_DEF:
            parse_memory(name, op);
            break;
        case AMOD_IND:
        case AMOD_IND_DEF:
            parse_indexed(name, op);
            break;
        default: 
            parse_branch(name, op);
            break;
    }
}

bool is_immediate(char *imm) {
    return imm[0] == '\'' || imm[0] == '-' || 
    (imm[0] == '0' && isxdigit((unsigned char)imm[1])) || isdigit((unsigned char)imm[0]);
}

bool is_rn(char *reg, int offset) {
    return reg[offset] == 'r' && reg[offset + 1] >= '0' && reg[offset + 1] <= '7';
}

bool is_register(char *reg) {
    return (is_rn(reg, 0) && reg[2] == '\0') ||
           (strcmp(reg, "sp") == 0) || (strcmp(reg, "pc") == 0);
}   

bool is_register_def(char *reg) {
    return (reg[0] == '(' && is_rn(reg, 1) && reg[3] == ')' && reg[4] == '\0') ||
           (strcmp(reg, "(sp)") == 0) || (strcmp(reg, "(pc)") == 0);
}

bool is_autoinc(char *reg) {
    return (reg[0] == '(' && is_rn(reg, 1) && reg[3] == ')' && reg[4] == '+' && reg[5] == '\0') || 
           (strcmp(reg, "(sp)+") == 0) || (strcmp(reg, "(pc)+") == 0);
}

bool is_autodec(char *reg) {
    return (reg[0] == '-' && reg[1] == '(' && is_rn(reg, 2) && reg[4] == ')' && reg[5] == '\0') || 
           (strcmp(reg, "-(sp)") == 0) || (strcmp(reg, "-(pc)") == 0);
}

void parse_derective(char *dir) {
    char name[8];
    sscanf(dir, "%7s", name);

    size_t len = strlen(name);
    if (dir[len] != ' ' && dir[len] != '\0') {
        diagnostic_add(dq, DIAGL_ERROR, lineno, "directive expected");
        return;
    }

    str_to_lower(name);
    if (!mnemonic_exists(&entry[instrno].instr, name)) {
        diagnostic_add(dq, DIAGL_ERROR, lineno, "directive expected");
        return;
    }

    dir += len;
    char *after_dir = skip_spaces(dir);
    int res = parse_dirops(after_dir);
    if (res == -1) {
        diagnostic_add(dq, DIAGL_ERROR, lineno, "invalid operand");
        return;
    } else if (res == -2) {
        diagnostic_free(dq);
        exit(EXIT_FAILURE);
    }
}

int parse_dirops(char *operands) {
    int res;
    uint16_t op_count = 0;
    int64_t value = 0;
    char *comma, *cur_op, *ptr = NULL;
    bool is_block = entry[instrno].instr.dir == DIR_BLKB || entry[instrno].instr.dir == DIR_BLKW;
    is_inc_mode = true;

    while (1) {
        cur_op = operands;
        comma = find_comma_outside(operands);
        if (comma) {
            if (is_block) return -1;
            *comma = '\0';
        }

        if (*cur_op == '"') {
            res = parse_string(cur_op + 1, &op_count, &ptr);
        } else if (is_immediate(cur_op)) {
            if (!is_block) {
                res = parse_byte_and_word(cur_op, &op_count, &ptr);
            } else if (*cur_op == '-') {
                res = -1;
            } else {
                res = parse_block(cur_op, &value);
            }
        } else res = -1;

        if (res != 0) {
            free(ptr);
            return res;
        }

        if (!comma) break;
        operands = skip_spaces(comma + 1);
    }

    if (is_block) {
        entry[instrno].instr.value = (uint16_t)value;
    } else {
        if (entry[instrno].instr.dir == DIR_BYTE && op_count % 2 != 0) {
            size_t tmp_s = op_count;
            op_count++;
            char *tmp = realloc(ptr, op_count);
            if (!tmp) {
                free(ptr);
                return -2;
            }
            addr++;
            ptr = tmp;
            ptr[tmp_s] = 0;
        }
        entry[instrno].instr.ptr = ptr;
        entry[instrno].instr.value = op_count;
    }

    instrno++;
    return 0;
}

int parse_string(char *string, uint16_t *op_count, char **ptr) {
    if (entry[instrno].instr.dir != DIR_BYTE) return -1;

    char *quot_mark = strchr(string, '"');
    if (!quot_mark) return -1;

    *quot_mark = '\0';
    quot_mark = skip_spaces(quot_mark + 1);
    if (*quot_mark != '\0') return -1;

    size_t len = strlen(string);
    if (len == 0) return -1;

    size_t tmp_s = *op_count;
    *op_count += len;
    char *tmp = realloc(*ptr, *op_count);
    if (!tmp) return -2;

    addr += len;
    *ptr = tmp;
    memcpy(*ptr + tmp_s, string, len);

    return 0;
}

int parse_value(char *imm, int64_t *value) {
    str_to_lower(imm);
    int base = 8;
    size_t len = strlen(imm);

    switch (imm[len - 1]) {
        case 'd': base = 10; imm[len - 1] = '\0'; break;
        case 'h': base = 16; imm[len - 1] = '\0'; break;
        case 'b': base = 2; imm[len - 1] = '\0'; break;
    }

    char *end;
    *value = strtoll(imm, &end, base);
    if (*end != '\0') return -1;

    if (entry[instrno].instr.is_byte && is_inc_mode) {
        if (*value < -128 || *value > 255) {
            diagnostic_add(dq, DIAGL_WARNING, lineno, "overflow in 8-bit immediate value");
        }
        *value &= 0xFF;
    } else {
        if (*value < -32768 || *value > 65535) {
            diagnostic_add(dq, DIAGL_WARNING, lineno, "overflow in 16-bit immediate value");
        }
        *value &= 0xFFFF;
    }

    return 0;
}

int parse_byte_and_word(char *str, uint16_t *op_count, char **ptr) {
    char *space = strchr(str, ' ');
    if (space) {
        *space = '\0';
        char *after_space = skip_spaces(space + 1);
        if (*after_space != '\0') return -1;
    }

    int64_t value;
    if (parse_value(str, &value) == -1) return -1;

    size_t tmp_s = *op_count;
    *op_count += entry[instrno].instr.is_byte ? 1 : 2;
    char *tmp = realloc(*ptr, *op_count);
    if (!tmp) return -2;

    *ptr = tmp;

    if (entry[instrno].instr.is_byte) {
        addr++;
        uint8_t byte = (uint8_t)value;
        memcpy(*ptr + tmp_s, &byte, 1);
    } else {
        addr += 2;
        uint16_t word = (uint16_t)value;
        memcpy(*ptr + tmp_s, &word, 2);
    }

    return 0;
}

int parse_block(char *str, int64_t *value) {
    char *space = strchr(str, ' ');
    if (space) {
        *space = '\0';
        char *after_space = skip_spaces(space + 1);
        if (*after_space != '\0') return -1;
    }

    if (parse_value(str, value) == -1 || *value == 0) return -1;
    
    if (entry[instrno].instr.dir == DIR_BLKB && *value % 2 != 0) (*value)++;

    addr += entry[instrno].instr.dir == DIR_BLKB ? *value : (*value << 1);

    return 0;
}

char *find_comma_outside(char *str) {
    bool in_quotes = false;
    while (*str) {
        if (*str == '"' || *str == '\'') {
            in_quotes = !in_quotes;
        } else if (*str == ',' && !in_quotes) {
            return str;
        }
        str++;
    }
    return NULL;
}

char *find_space_outside(char *str) {
    bool in_quotes = false;
    while (*str) {
        if (*str == '"' || *str == '\'') {
            in_quotes = !in_quotes;
        } else if (*str == ' ' && !in_quotes) {
            return str;
        }
        str++;
    }
    return NULL;
}
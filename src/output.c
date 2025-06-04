#include "output.h"

uint16_t output[MAX_INSTR_COUNT];
uint32_t cur_instr;

int main_output(char *filename) {
    int fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0755);
    if (fd == -1) return -1;

    for (uint32_t i = 0; i < instrno; i++) {
        struct instr_info *instruction = &entry[i].instr;

        switch (instruction->type) {
            case INSTRT_DOUBLE:
                output[cur_instr++] = output_instrd(instruction->instrd, &entry[i].op1, &entry[i].op2);
                extra_instr(&entry[i].op1);
                extra_instr(&entry[i].op2);
                break;
            case INSTRT_SINGLE:
                output[cur_instr++] = output_instrs(instruction->instrs, &entry[i].op1);
                extra_instr(&entry[i].op1);
                break;
            case INSTRT_BRANCH:
                output[cur_instr++] = output_instrb(instruction->instrb, &entry[i].op1);
                break;
            case INSTRT_WITHOUT:
                output[cur_instr++] = output_instrw(instruction->instrw);
                break;
            case INSTRT_DIRECTIVE: break;
        }
    }

    write_le16(fd, 0x0001);
    write_le16(fd, cur_instr * sizeof(uint16_t) + 6);
    write_le16(fd, 01000);
    write(fd, output, cur_instr * sizeof(uint16_t));
    uint8_t checksum = calculate_checksum(cur_instr * sizeof(uint16_t) + 6, 01000);
    write(fd, &checksum, sizeof(uint8_t));

    write_le16(fd, 0x0001);
    write_le16(fd, 0x0006);
    write_le16(fd, 01000);

    close(fd);
    
    return 0;
}

uint16_t output_instrd(enum instr_double type, struct operand *op1, struct operand *op2) {
    uint16_t bin_code = (uint16_t)((addr_mode(op1) << 6) | addr_mode(op2));
    uint16_t opcode;

    switch (type) {
        case INSTRD_MOV: opcode = get_opcode("mov"); break;
        case INSTRD_MOVB: opcode = get_opcode("movb"); break;
        case INSTRD_CMP: opcode = get_opcode("cmp"); break;
        case INSTRD_CMPB: opcode = get_opcode("cmpb"); break;
        case INSTRD_BIT:  opcode = get_opcode("bit"); break;
        case INSTRD_BITB: opcode = get_opcode("bitb"); break;
        case INSTRD_BIC: opcode = get_opcode("bic"); break;
        case INSTRD_BICB: opcode = get_opcode("bicb"); break;
        case INSTRD_BIS: opcode = get_opcode("bis"); break;
        case INSTRD_BISB: opcode = get_opcode("bisb"); break;
        case INSTRD_ADD: opcode = get_opcode("add"); break;
        case INSTRD_SUB: opcode = get_opcode("movb"); break;
        case INSTRD_JSR: opcode = get_opcode("jsr"); break;
    }

    if (type == INSTRD_JSR) {
        bin_code = (opcode << 9) | bin_code;
    } else {
        bin_code = (opcode << 12) | bin_code;
    }

    return bin_code;
}

uint16_t output_instrs(enum instr_single type, struct operand *op) {
    uint16_t bin_code = addr_mode(op);
    uint16_t opcode;

    switch (type) {
        case INSTRS_JMP: opcode = get_opcode("jmp"); break;
        case INSTRS_SWAB: opcode = get_opcode("swab"); break;
        case INSTRS_CLR: opcode = get_opcode("clr"); break;
        case INSTRS_CLRB: opcode = get_opcode("clrb"); break;
        case INSTRS_COM: opcode = get_opcode("com"); break;
        case INSTRS_COMB: opcode = get_opcode("comb"); break;
        case INSTRS_INC: opcode = get_opcode("inc"); break;
        case INSTRS_INCB: opcode = get_opcode("incb"); break;
        case INSTRS_DEC: opcode = get_opcode("dec"); break;
        case INSTRS_DECB: opcode = get_opcode("decb"); break;
        case INSTRS_NEG: opcode = get_opcode("neg"); break;
        case INSTRS_NEGB: opcode = get_opcode("negb"); break;
        case INSTRS_ADC: opcode = get_opcode("adc"); break;
        case INSTRS_ADCB: opcode = get_opcode("abcb"); break;
        case INSTRS_SBC: opcode = get_opcode("sbc"); break;
        case INSTRS_SBCB: opcode = get_opcode("sbcb"); break;
        case INSTRS_TST: opcode = get_opcode("tst"); break;
        case INSTRS_TSTB: opcode = get_opcode("tstb"); break;
        case INSTRS_ROR: opcode = get_opcode("ror"); break;
        case INSTRS_RORB: opcode = get_opcode("rorb"); break;
        case INSTRS_ROL: opcode = get_opcode("rol"); break;
        case INSTRS_ROLB: opcode = get_opcode("rolb"); break;
        case INSTRS_ASR: opcode = get_opcode("asr"); break;
        case INSTRS_ASRB: opcode = get_opcode("asrb"); break;
        case INSTRS_ASL: opcode = get_opcode("asl"); break;
        case INSTRS_ASLB: opcode = get_opcode("aslb"); break;
        case INSTRS_MTPS: opcode = get_opcode("mtps"); break;
        case INSTRS_MFPS: opcode = get_opcode("mfps"); break;
        case INSTRS_RTS: opcode = get_opcode("rts"); break;
    }

    if (type == INSTRS_RTS) {
        bin_code = (opcode << 3) | bin_code;
    } else {
        bin_code = (opcode << 6) | bin_code;
    }

    return bin_code;
}

uint16_t output_instrb(enum instr_branch type, struct operand *op) {
    uint16_t bin_code = (op->mem_off & 0xFF);
    uint16_t opcode;

    switch (type) {
        case INSTRB_BR: opcode = get_opcode("br"); break;
        case INSTRB_BNE: opcode = get_opcode("bne"); break;
        case INSTRB_BEQ: opcode = get_opcode("beq"); break;
        case INSTRB_BGE: opcode = get_opcode("bge"); break;
        case INSTRB_BLT: opcode = get_opcode("blt"); break;
        case INSTRB_BGT: opcode = get_opcode("bgt"); break;
        case INSTRB_BLE: opcode = get_opcode("ble"); break;
        case INSTRB_BPL: opcode = get_opcode("bpl"); break;
        case INSTRB_BMI: opcode = get_opcode("bmi"); break;
        case INSTRB_BHI: opcode = get_opcode("bhi"); break;
        case INSTRB_BLOS: opcode = get_opcode("blos"); break;
        case INSTRB_BHIS: opcode = get_opcode("bhis"); break;
        case INSTRB_BLO: opcode = get_opcode("blo"); break;
        case INSTRB_BVC: opcode = get_opcode("bvc"); break;
        case INSTRB_BVS: opcode = get_opcode("bvs"); break;
        case INSTRB_BCC: opcode = get_opcode("bcc"); break;
        case INSTRB_BCS: opcode = get_opcode("bcs"); break;
    }

    bin_code = (opcode << 8) | bin_code;

    return bin_code;
}

uint16_t output_instrw(enum instr_without type) {
    switch (type) {
        case INSTRW_CLC: return get_opcode("clc");
        case INSTRW_CLV: return get_opcode("clv");
        case INSTRW_CLZ: return get_opcode("clz");
        case INSTRW_CLN: return get_opcode("cln");
        case INSTRW_CCC: return get_opcode("ccc");
        case INSTRW_SEC: return get_opcode("sec");
        case INSTRW_SEV: return get_opcode("sev");
        case INSTRW_SEZ: return get_opcode("sez");
        case INSTRW_SEN: return get_opcode("sen");
        case INSTRW_SCC: return get_opcode("scc");
        case INSTRW_HALT: return get_opcode("halt");
        case INSTRW_WAIT: return get_opcode("wait");
        case INSTRW_NOP: return get_opcode("nop");
        default: return UINT16_MAX;
    }
}

uint8_t addr_mode(struct operand *op) {
    uint8_t addrmode = 0;

    switch (op->mode) {
        case AMOD_REG: addrmode = 0; break;
        case AMOD_REG_DEF: addrmode = 1; break;
        case AMOD_INC: addrmode = 2; break;
        case AMOD_INC_DEF: addrmode = 3; break;
        case AMOD_DEC: addrmode = 4; break;
        case AMOD_DEC_DEF: addrmode = 6; break;
        case AMOD_IND: addrmode = 6; break;
        case AMOD_IND_DEF: addrmode = 7; break;
    }

    return (addrmode << 3) | op->regno;
}

uint16_t get_opcode(const char *mnemonic) {
    size_t len = strlen(mnemonic);

    for (uint32_t i = 0; i < instructions_size; i++) {
        if (strlen(instructions[i].mnemonic) != len) continue;
        if (memcmp(instructions[i].mnemonic, mnemonic, len) == 0) return instructions[i].opcode;
    }

    return UINT16_MAX;
}

void extra_instr(struct operand *op) {
    if (op->type == OPT_IMM) {
        output[cur_instr++] = op->imm;
    } else if (op->type == OPT_MEM) {
        output[cur_instr++] = op->mem_off;
    } else {
        if (op->mode == AMOD_IND || op->mode == AMOD_IND_DEF) {
            output[cur_instr++] = op->mem_off;
        }
    }
}

int write_le16(int fd, uint16_t word) {
    uint8_t bytes[2];
    bytes[0] = word & 0xFF;
    bytes[1] = (word >> 8) & 0xFF;

    ssize_t written = write(fd, bytes, 2);
    return (written == 2) ? 0 : -1;
}

uint8_t calculate_checksum(uint16_t len, uint16_t addr) {
    uint8_t sum = 0;

    sum += 0x01;
    sum += 0x00;

    sum += len & 0xFF;
    sum += (len >> 8) & 0xFF;

    sum += addr & 0xFF;
    sum += (addr >> 8) & 0xFF;

    for (uint32_t i = 0; i < cur_instr; i++) {
        sum += output[i] & 0xFF;
        sum += (output[i] >> 8) & 0xFF;
    }

    return (uint8_t)(-sum);
}
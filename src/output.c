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
                output[cur_instr++] = output_instrd(instruction->info, &entry[i].op1, &entry[i].op2, instruction->opcode);
                extra_instr(&entry[i].op1);
                extra_instr(&entry[i].op2);
                break;
            case INSTRT_SINGLE:
                output[cur_instr++] = output_instrs(instruction->info, &entry[i].op1, instruction->opcode);
                extra_instr(&entry[i].op1);
                break;
            case INSTRT_BRANCH:
                output[cur_instr++] = output_instrb(&entry[i].op1, instruction->opcode);
                break;
            case INSTRT_WITHOUT:
                output[cur_instr++] = instruction->opcode;
                break;
            case INSTRT_DIRECTIVE:
                output_directive(instruction);
                break;
            default: break;
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

uint16_t output_instrd(enum instr_extra type, struct operand *op1, struct operand *op2, uint16_t opcode) {
    uint16_t bin_code = (uint16_t)((addr_mode(op1) << 6) | addr_mode(op2));

    if (type == INSTR_JSR) {
        bin_code = (opcode << 9) | bin_code;
    } else {
        bin_code = (opcode << 12) | bin_code;
    }

    return bin_code;
}

uint16_t output_instrs(enum instr_extra type, struct operand *op, uint16_t opcode) {
    uint16_t bin_code = addr_mode(op);

    if (type == INSTR_RTS) {
        bin_code = (opcode << 3) | bin_code;
    } else {
        bin_code = (opcode << 6) | bin_code;
    }

    return bin_code;
}

uint16_t output_instrb(struct operand *op, uint16_t opcode) {
    uint16_t bin_code = (op->mem_off & 0xFF);
    bin_code = (opcode << 8) | bin_code;
    return bin_code;
}

void output_directive(struct instr_info *instruction) {
    enum directive_type type = instruction->dir;
    uint16_t value = instruction->value;
    uint16_t *ptr = (uint16_t *)instruction->ptr;

    switch (type) {
        case DIR_BYTE:
        case DIR_WORD: {
            for (uint16_t i = 0; i < (value >> 1); i++) {
                output[cur_instr++] = ptr[i];
            }
            break;
        }  
        case DIR_BLKB:
            value >>= 1;
            /* fall through */
        case DIR_BLKW:
            for (uint16_t i = 0; i < value; i++) {
                output[cur_instr++] = 0;
            }
            break;
        default: break;
    }
    free(ptr);
}

uint8_t addr_mode(struct operand *op) {
    uint8_t addrmode;

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
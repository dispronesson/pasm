#ifndef OUTPUT_H
#define OUTPUT_H

#define _POSIX_C_SOURCE 200809L

#include "parser.h"
#include <fcntl.h>

int main_output(char *filename);
uint16_t output_instrd(enum instr_extra type, struct operand *op1, struct operand *op2, uint16_t opcode);
uint16_t output_instrs(enum instr_extra type, struct operand *op, uint16_t opcode);
uint16_t output_instrb(struct operand *op, uint16_t opcode);
void output_directive(struct instr_info *instruction);
uint8_t addr_mode(struct operand *op);
void extra_instr(struct operand *op);
int write_le16(int fd, uint16_t word);
uint8_t calculate_checksum(uint16_t len, uint16_t addr);

#endif //OUTPUT_H
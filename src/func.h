#ifndef FUNC_H
#define FUNC_H

#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>

extern char *input_file;
extern char *output_file;

void str_to_lower(char *str);
void replace_char(char *str, char old_char, char new_char);
char *skip_spaces(char *str);
bool is_valid_name(const char *str);
bool starts_with(const char *s, const char *start);
void valid_args(int argc, char **argv);
void print_help();

#endif //FUNC_H
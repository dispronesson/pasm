#ifndef FUNC_H
#define FUNC_H

#include <ctype.h>
#include <stdbool.h>
#include <string.h>

void str_to_lower(char *str);
void replace_char(char *str, char old_char, char new_char);
char *skip_spaces(char *str);
bool is_valid_name(const char *str);
bool starts_with(const char *s, const char *start);

#endif //FUNC_H
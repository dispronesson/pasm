#ifndef FUNC_H
#define FUNC_H

#include <ctype.h>

void str_to_lower(char *str);
void replace_char(char *str, char old_char, char new_char);
char *skip_spaces(char *str);
int is_valid_name(const char *str);

#endif //FUNC_H
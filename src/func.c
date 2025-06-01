#include "func.h"

void str_to_lower(char *str) {
    while (*str) {
        *str = tolower((unsigned char)*str);
        str++;
    }
}

void replace_char(char *str, char old_char, char new_char) {
    while (*str) {
        if (*str == old_char) *str = new_char;
        str++;
    }
}

char *skip_spaces(char *str) {
    while (*str == ' ') str++;
    return str; 
}

int is_valid_name(const char *str) {
    if (!isalpha((unsigned char)str[0]) && str[0] != '_') return 0;
    for (int i = 1; str[i]; i++) {
        if (!isalnum((unsigned char)str[i]) && str[i] != '_') return 0;
    } 
    return 1;
}

bool starts_with(const char *s, const char *start) {
    return strncmp(s, start, strlen(start)) == 0;
}
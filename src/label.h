#ifndef LABEL_H
#define LABEL_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

#define MAX_LABEL_COUNT 512
#define MAX_LABEL_LENGTH 16

struct label {
    char name[MAX_LABEL_LENGTH + 1];
    size_t addr;
};

struct label_table {
    struct label labels[MAX_LABEL_COUNT];
    size_t count;
};

bool label_exists(struct label_table *ltab, const char *name);
int label_add(struct label_table *ltab, const char *name);
void set_label_addr(struct label_table *ltab, const char *name, size_t addr);
size_t get_label_addr(struct label_table *ltab, const char *name);

#endif //LABEL_H
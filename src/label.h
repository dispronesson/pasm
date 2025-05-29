#ifndef LABEL_H
#define LABEL_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_LABEL_COUNT 512
#define MAX_LABEL_NAME 16

struct label {
    char name[MAX_LABEL_NAME + 1];
    size_t addr;
};

struct label_table {
    struct label labels[MAX_LABEL_COUNT];
    size_t count;
};

int label_exists(struct label_table *label_tab, const char *name);
int label_add(struct label_table *label_tab, const char *name, size_t addr);

#endif //LABEL_H
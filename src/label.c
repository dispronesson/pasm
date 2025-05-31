#include "label.h"

int label_exists(struct label_table *ltab, const char *name) {
    for (size_t i = 0; i < ltab->count; i++) {
        if (strcmp(ltab->labels[i].name, name) == 0) return 1;
    }

    return 0;
}

int label_add(struct label_table *ltab, const char *name, size_t addr) {
    if (ltab->count >= MAX_LABEL_COUNT) return -1;

    snprintf(ltab->labels[ltab->count].name, MAX_LABEL_LENGTH + 1, "%s", name);
    ltab->labels[ltab->count].addr = addr;
    ltab->count++;

    return 0;
}

size_t get_label_addr(struct label_table *ltab, const char *name) {
    for (size_t i = 0; i < ltab->count; i++) {
        if (strcmp(ltab->labels[i].name, name) == 0) return ltab->labels[i].addr;
    }

    return (size_t)UINT64_MAX;
}
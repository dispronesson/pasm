#include "label.h"

bool label_exists(struct label_table *ltab, const char *name) {
    for (size_t i = 0; i < ltab->count; i++) {
        if (strcmp(ltab->labels[i].name, name) == 0) return true;
    }

    return false;
}

int label_add(struct label_table *ltab, const char *name) {
    if (ltab->count >= MAX_LABEL_COUNT) return -1;

    snprintf(ltab->labels[ltab->count].name, MAX_LABEL_LENGTH + 1, "%s", name);
    ltab->labels[ltab->count].addr = 0;
    ltab->count++;

    return 0;
}

void set_label_addr(struct label_table *ltab, const char *name, size_t addr) {
    for (size_t i = 0; i < ltab->count; i++) {
        if (strcmp(ltab->labels[i].name, name) == 0) {
            ltab->labels[i].addr = addr; break;
        }
    }
}

size_t get_label_addr(struct label_table *ltab, const char *name) {
    for (size_t i = 0; i < ltab->count; i++) {
        if (strcmp(ltab->labels[i].name, name) == 0) return ltab->labels[i].addr;
    }

    return (size_t)UINT64_MAX;
}
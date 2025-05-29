#include "label.h"

static int label_exists(struct label_table *label_tab, const char *name) {
    for (size_t i = 0; i < label_tab->count; i++) {
        if (strcmp(label_tab->labels[i].name, name) == 0) return 1;
    }

    return 0;
}

int label_add(struct label_table *label_tab, const char *name, size_t addr) {
    if (label_tab->count >= MAX_LABEL_COUNT) return -1;
    if (label_exists(label_tab, name)) return -2;

    snprintf(label_tab->labels[label_tab->count].name, MAX_LABEL_NAME + 1, "%s", name);
    label_tab->labels[label_tab->count].addr = addr;
    label_tab->count++;

    return 0;
}
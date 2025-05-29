#include "diagnostic.h"

DiagnosticQueue *create_diagnostic_queue() {
    DiagnosticQueue *q = malloc(sizeof(DiagnosticQueue));
    if (!q) return NULL;

    q->head = NULL;
    q->tail = NULL;

    return q;
}

int diagnostic_add(DiagnosticQueue *q, DiagnosticLevel level, uint16_t line, const char *fmt, ...) {
    if (!q) return -1;

    Diagnostic *d = malloc(sizeof(Diagnostic));
    if (!d) return -1;

    d->level = level;
    d->line = line;
    d->next = NULL;

    va_list list;
    va_start(list, fmt);
    vsnprintf(d->msg, sizeof(d->msg), fmt, list);
    va_end(list);

    if (!q->tail) {
        q->head = d;
        q->tail = d;
    }
    else {
        q->tail->next = d;
        q->tail = d;
    }

    return 0;
}

void diagnostic_print(DiagnosticQueue *q) {
    if (!q) return;

    Diagnostic *d = q->head;
    while (d) {
        const char *level_str;
        switch (d->level) {
            case DIAG_WARNING: level_str = "warning"; break;
            case DIAG_ERROR: level_str = "error"; break;
            default: level_str = "unknown"; break;
        }

        fprintf(stderr, "line %u: %s: %s\n", d->line, level_str, d->msg);
        d = d->next;
    }
}

void diagnostic_free(DiagnosticQueue *q) {
    if (!q) return;

    Diagnostic *d = q->head;
    while (d) {
        Diagnostic *temp = d->next;
        free(d);
        d = temp;
    }

    free(q);
}
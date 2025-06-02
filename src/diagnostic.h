#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_MSG_SIZE 128

enum diagnostic_level {
    DIAGL_WARNING,
    DIAGL_ERROR
};

struct diagnostic {
    enum diagnostic_level level;
    uint16_t line;
    char msg[MAX_MSG_SIZE + 1];
    struct diagnostic *next;
};

struct diagnostic_queue {
    struct diagnostic *head;
    struct diagnostic *tail;
};

struct diagnostic_queue *create_diagnostic_queue();
int diagnostic_add(struct diagnostic_queue *q, enum diagnostic_level level, uint16_t line, const char *fmt, ...);
void diagnostic_print(struct diagnostic_queue *q);
void diagnostic_free(struct diagnostic_queue *q);
bool has_errors(struct diagnostic_queue *q);

#endif //DIAGNOSTIC_H
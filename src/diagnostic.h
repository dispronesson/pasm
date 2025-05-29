#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define MSG_SIZE 128

typedef enum DiagnosticLevel {
    DIAG_WARNING,
    DIAG_ERROR
} DiagnosticLevel;

typedef struct Diagnostic {
    DiagnosticLevel level;
    uint16_t line;
    char msg[MSG_SIZE];
    struct Diagnostic *next;
} Diagnostic;

typedef struct DiagnosticQueue {
    Diagnostic *head;
    Diagnostic *tail;
} DiagnosticQueue;

DiagnosticQueue *create_diagnostic_queue();
int diagnostic_add(DiagnosticQueue *q, DiagnosticLevel level, uint16_t line, const char *fmt, ...);
void diagnostic_print(DiagnosticQueue *q);
void diagnostic_free(DiagnosticQueue *q);

#endif //DIAGNOSTIC_H
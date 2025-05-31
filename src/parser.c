#include "parser.h"
#include "label.h"
#include "diagnostic.h"
#include "func.h"

struct diagnostic_queue *dq;
struct label_table ltab = {0};
struct instr_entry entry[MAX_INSTR_COUNT];
uint32_t lineno = 1;
size_t addr = BASE_ADDR;
FILE *input;

int read_file(const char *filename) {
    input = fopen(filename, "r");
    if (!input) return -1;

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), input)) {
        char *pos = strchr(buffer, '\n');
        if (pos) {
            *pos = '\0';
        } else {
            int c;
            while ((c = fgetc(input)) != '\n' && c != EOF);
        }

        lineno++;
        replace_char(buffer, '\t', ' ');
        char *line = skip_spaces(buffer);
        if (line[0] == '\0' || line[0] == ';') continue;

        parse_line(line);
    }

    return 0;
}

void parse_line(char *line) {
    char *after_label = parse_label(line);
    if (after_label && *after_label != '\0') line = after_label;
}

char *parse_label(char *label) {
    char *after_label = NULL;
    char *colon = strchr(label, ':');
    if (colon) {
        if (*(colon + 1) != ' ' && *(colon + 1) != '\0') {
            diagnostic_add(dq, DIAGL_ERROR, lineno, "invalid label definition");
        } else {
            *colon = '\0';
            str_to_lower(label);
            if (label[0] == '\0') {
                diagnostic_add(dq, DIAGL_ERROR, lineno, "label name expected");
            } else if (strlen(label) > MAX_LABEL_LENGTH) {
                diagnostic_add(dq, DIAGL_ERROR, lineno, "max 16 chars label length");
            } else if (!is_valid_name(label)) {
                diagnostic_add(dq, DIAGL_ERROR, lineno, "invalid label name");
            } else if (label_exists(&ltab, label)) {
                diagnostic_add(dq, DIAGL_ERROR, lineno, "label '%s' redefined", label);
            } else if (label_add(&ltab, label, addr) == -1) {
                diagnostic_add(dq, DIAGL_ERROR, lineno, "max 512 label count");
            }
        }

        after_label = skip_spaces(colon + 1);
    }
 

    return after_label;
}
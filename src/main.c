#include "output.h"
#include "parser.h"
#include "diagnostic.h"

int main(int argc, char *argv[]) {
    if (argc != 3) return -1;

    dq = create_diagnostic_queue();
    if (!dq) {
        perror("malloc");
        return -1;
    }

    if (read_file(argv[1]) == -1) {
        perror("fopen");
        diagnostic_free(dq);
        return -1;
    }

    if (has_errors(dq)) {
        diagnostic_print(dq);
        diagnostic_free(dq);
        return -1;
    }

    diagnostic_print(dq);
    diagnostic_free(dq);

    if (main_output(argv[2]) == -1) {
        perror("open");
        return -1;
    }

    return 0;
}
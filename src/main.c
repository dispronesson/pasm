#include "output.h"
#include "parser.h"
#include "diagnostic.h"
#include "func.h"

int main(int argc, char *argv[]) {
    valid_args(argc, argv);

    if (!input_file) {
        fprintf(stderr, "pasm: fatal: no input file specified\n");
        fprintf(stderr, "type pasm -h for help\n");
        return 1;
    }

    dq = create_diagnostic_queue();
    if (!dq) {
        fprintf(stderr, "pasm: fatal: not enough memory\n");
        return 1;
    }

    if (read_file(input_file) == -1) {
        perror("pasm: fatal");
        diagnostic_free(dq);
        return 1;
    }

    if (main_output(output_file) == -1) {
        perror("pasm: fatal");
        return 1;
    }

    return 0;
}
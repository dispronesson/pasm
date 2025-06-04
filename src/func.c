#include "func.h"

char *input_file = NULL;
char *output_file = "output";

void str_to_lower(char *str) {
    while (*str) {
        *str = tolower((unsigned char)*str);
        str++;
    }
}

void replace_char(char *str, char old_char, char new_char) {
    while (*str) {
        if (*str == old_char) *str = new_char;
        str++;
    }
}

char *skip_spaces(char *str) {
    while (*str == ' ') str++;
    return str; 
}

bool is_valid_name(const char *str) {
    if (!isalpha((unsigned char)str[0]) && str[0] != '_') return false;
    for (int i = 1; str[i]; i++) {
        if (!isalnum((unsigned char)str[i]) && str[i] != '_') return false;
    } 
    return true;
}

bool starts_with(const char *s, const char *start) {
    return strncmp(s, start, strlen(start)) == 0;
}

void valid_args(int argc, char **argv) {
    int opt;
    static struct option options[] = {
        { "help", no_argument, NULL, 'h' },
        { "version", no_argument, NULL, 'v' },
        { 0, 0, 0, 0 }
    };

    while ((opt = getopt_long(argc, argv, ":hvi:o:", options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                exit(EXIT_SUCCESS);
            case 'v':
                printf("PASM version 0.0.1\n");
                exit(EXIT_SUCCESS);
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case ':':
                if (optopt == 'i') {
                    fprintf(stderr, "pasm: fatal: no input file specified\n");
                } else if (optopt == 'o') {
                    fprintf(stderr, "pasm: error: option '-o' requires an argument\n");
                }
                fprintf(stderr, "type pasm -h for help\n");
                exit(EXIT_FAILURE);
                break;
            case '?':
                if (strncmp(argv[optind - 1], "--", 2) == 0) {
                    fprintf(stderr, "pasm: error: unknown option '%s'\n", argv[optind - 1]);
                } else {
                    fprintf(stderr, "pasm: error: unknown option '-%c'\n", optopt);
                }
                fprintf(stderr, "type pasm -h for help\n");
                exit(EXIT_FAILURE);
            default: break;
        }
    }
}

void print_help() {
    printf("Usage: pasm -i input_file -o filename\n");
    printf("       pasm [options...]\n");

    printf("\nOptions:\n");
    printf("    %-15s %s\n", "-h, --help", "show this text and exit");
    printf("    %-15s %s\n", "-v, --version", "print the PASM version and exit");
    printf("    %-15s %s\n", "-i file", "input file");
    printf("    %-15s %s\n", "-o outfile", "write output to outfile; by default use name 'output'");
}
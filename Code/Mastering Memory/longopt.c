#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

int main(int argc, char *argv[]) {
    int opt;
    int option_index = 0;

    static struct option long_options[] = {
        {"help",    no_argument,       0, 'h'},
        {"output",  required_argument, 0, 'o'},
        {"verbose", no_argument,       0, 'v'},
    };

    while ((opt = getopt_long(argc, argv, "ho:v", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                printf("Usage: %s [--help] [--output FILE] [--verbose]\n", argv[0]);
                exit(0);
            case 'o':
                printf("Output file: %s\n", optarg);
                break;
            case 'v':
                printf("Verbose mode enabled\n");
                break;
            case '?':
                printf("Try '%s --help' for usage.\n", argv[0]);
                exit(1);
            default:
                break;
        }
    }

    if (optind < argc) {
        printf("Non-option arguments:\n");
        while (optind < argc) {
            printf("  %s\n", argv[optind++]);
        }
    }

    return 0;
}

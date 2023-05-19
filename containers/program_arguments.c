#include "program_arguments.h"
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

bool parse_option_t(const char *option_parameter, enum waste_types waste_types[WASTE_TYPES]) {
    for (int waste_type = 0; waste_type < (int) strlen(option_parameter); ++waste_type) {
        switch (option_parameter[waste_type]) {
            case 'A':
                waste_types[PLASTICS_AND_ALUMINIUM] = PLASTICS_AND_ALUMINIUM;
                break;
            case 'P':
                waste_types[PAPER] = PAPER;
                break;
            case 'B':
                waste_types[BIODEGRADABLE_WASTE] = BIODEGRADABLE_WASTE;
                break;
            case 'G':
                waste_types[CLEAR_GLASS] = CLEAR_GLASS;
                break;
            case 'C':
                waste_types[COLORED_GLASS] = COLORED_GLASS;
                break;
            case 'T':
                waste_types[TEXTILE] = TEXTILE;
                break;
            default:
                return false;
        }
    }
    return true;
}

bool parse_option_p(const char *option_parameter, bool *is_public) {
    if (strcmp(option_parameter, "Y") == 0) {
        *is_public = true;
        return true;
    }
    if (strcmp(option_parameter, "N") == 0) {
        *is_public = false;
        return true;
    }
    return false;
}

bool parse_option_cg(int option, const char *option_parameter, unsigned int *first, unsigned int *second, char delimiter) {
    if (option != OPT_C && option != OPT_G) {
        return false;
    }
    char *end;
    long num = strtol(option_parameter, &end, 10);
    if (errno != 0 || num < 0 || num >= UINT_MAX || *end != delimiter || end[1] == '\0') {
        return false;
    }
    *first = num;
    num = strtol(end + 1, &end, 10);
    if (errno != 0 || num < 0 || num >= UINT_MAX || *end != '\0') {
        return false;
    }
    *second = num;
    return option != OPT_C || *second >= *first;
}

bool check_exclusive_options(const bool options[OPTIONS]) {
    int opt_count = 0;
    if (options[OPT_S] || options[OPT_G]) {
        for (int option = 0; option < OPTIONS; ++option) {
            if (options[option]) {
                opt_count++;
            }
        }
        if (opt_count != 1) {
            return false;
        }
    }
    return true;
}

bool parse_options(int argc, char * const argv[], struct options *options) {
    if (argc < 3) {
        return false;
    }
    int arg = 1;
    for (; arg < argc - 2; ++arg) {
        if (!options->options[OPT_T] && strcmp(argv[arg], "-t") == 0 && parse_option_t(argv[arg + 1], options->waste_types)) {
            arg++;
            options->options[OPT_T] = true;
        } else if (!options->options[OPT_C] && strcmp(argv[arg], "-c") == 0 && parse_option_cg(OPT_C, argv[arg + 1], &options->first, &options->second, '-')) {
            arg++;
            options->options[OPT_C] = true;
        } else if (!options->options[OPT_P] && strcmp(argv[arg], "-p") == 0 && parse_option_p(argv[arg + 1], &options->is_public)) {
            arg++;
            options->options[OPT_P] = true;
        } else if (!options->options[OPT_S] && strcmp(argv[arg], "-s") == 0) {
            options->options[OPT_S] = true;
        } else if (!options->options[OPT_G] && strcmp(argv[arg], "-g") == 0 && parse_option_cg(OPT_G, argv[arg + 1], &options->first, &options->second, ',')) {
            arg++;
            options->options[OPT_G] = true;
        } else {
            return false;
        }
    }
    if (argv[arg] != argv[argc - 2] || !check_exclusive_options(options->options)) {
        return false;
    }
    return true;
}

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../common/utils/utils.h"
#include "parser.h"

bool valid_server_options(int argc, char **argv) {

    if (argc != 4 && argc != 6)
        return false;

    int opt = 0;
    while ((opt = getopt(argc, argv, "t:l:")) != -1) {
        switch (opt) {
            case 't':
                if (!is_all_digits(optarg) || optind != 3)
                    return false;
                break;
            case 'l':
                if (!is_all_digits(optarg) || optind != 5)
                    return false;
                break;
            default:
                return false;
        }
    }

    return true;
}
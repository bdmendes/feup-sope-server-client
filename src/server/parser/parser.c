#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "parser.h"
#include "../../common/utils/utils.h"

bool valid_server_options(int argc, char **argv){
    
    if(argc != 4 && argc != 6) return false;

    int opt = 0;
    bool hasNonOptionalArg = false;

    while ((opt = getopt(argc, argv, "t:l:")) != -1) {
        switch (opt) {
        case 't':
            if (!is_all_digits(optarg) || optind != 3) return false;
            hasNonOptionalArg = true;
            break;
        case 'l':
            if (!is_all_digits(optarg) || optind != 5 || !hasNonOptionalArg) return false;
            break;
        default:
            return false;
        }
    }

    return true;
}
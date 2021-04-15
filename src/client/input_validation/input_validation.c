#include <unistd.h>

#include "../../common/utils/utils.h"
#include "input_validation.h"

bool valid_client_options(int argc, char **argv) {
    if (argc != 4)
        return false;
    int opt = getopt(argc, argv, "t:");
    return opt == 't' && optind == 3 && is_all_digits(optarg);
}
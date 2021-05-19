#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "../../common/utils/utils.h"

bool valid_server_options(int argc, char **argv){
    
    //validate non-optional command args
    if((argc != 4 && argc != 6) || strcmp(argv[1], "-t")|| !is_all_digits(argv[2])) return false;

    //validate option command arg
    if((argc == 6) && (strcmp(argv[3], "-l") || !is_all_digits(argv[4]))) return false;

    return true;
}
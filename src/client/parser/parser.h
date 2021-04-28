#ifndef CLIENT_INPUT_VALIDATION_H
#define CLIENT_INPUT_VALIDATION_H

#include <stdbool.h>

/**
 * @brief Function to validate the client command line arguments:
 * "<executable> -t <timeout> <fifoname>"
 * @param argc Argc of main client
 * @param argv Argv of main client
 * @return True if the options are valid, false otherwise
 */
bool valid_client_options(int argc, char **argv);

#endif
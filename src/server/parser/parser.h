#ifndef SERVER_INPUT_VALIDATION_H
#define SERVER_INPUT_VALIDATION_H

#include <stdbool.h>

/**
 * @brief Function to checks if the options are valid according to the specs
 * @param argc Argc passed to main
 * @param argv Argv passed to main
 * @return True if the options are valid, false otherwise
 */
bool valid_server_options(int argc, char **argv);

#endif
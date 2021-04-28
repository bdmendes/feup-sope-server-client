#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <stdbool.h>

/**
 * @brief Auxiliar function that returns whether a string is composed just by digits or not 
 * @param str String to evaluate
 * @return True if the string is composed just by digits, false otherwise
 */
bool is_all_digits(char str[]);

#endif
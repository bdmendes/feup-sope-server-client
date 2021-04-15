#include "utils.h"
#include <ctype.h>
#include <string.h>

bool is_all_digits(char str[]) {
    for (int i = strlen(str) - 1; i >= 0; i--) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}
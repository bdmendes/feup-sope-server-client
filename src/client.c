#include <stdio.h>
#include "op_register.h"

int main() {
  OPERATION ot = IWANT;
  op_reg(4, 5, ot, 6);
  return 0;
}
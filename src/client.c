#include <stdio.h>
#include "op_register.h"

int main() {
  OPERATION ot = IWANT;
  op_reg(ot);
  printf("hello world!");
  return 0;
}
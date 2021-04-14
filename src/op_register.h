#ifndef OP_REGISTER_H
#define OP_REGISTER_H

typedef enum operation {IWANT, RECVD, TSKEX, TSKDN, GOTRS, LATE2, CLOSD, GAVUP, FAILD};

void op_reg(enum operation op);

#endif
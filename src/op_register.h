#ifndef OP_REGISTER_H
#define OP_REGISTER_H

 typedef enum OPERATION {IWANT, RECVD, TSKEX, TSKDN, GOTRS, LATE2, CLOSD, GAVUP, FAILD} OPERATION;

int op_reg(int i, int t, OPERATION op, int res);

#endif
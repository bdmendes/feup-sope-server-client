#ifndef LOGS_H
#define LOGS_H

typedef enum {IWANT, RECVD, TSKEX, TSKDN, GOTRS, TOOLATE, CLOSD, GAVUP, FAILD} OPERATION;

int op_reg(int i, int t, OPERATION op, int res);

#endif
#ifndef LOGS_H
#define LOGS_H

typedef enum {IWANT, RECVD, TSKEX, TSKDN, GOTRS, TOOLATE, CLOSD, GAVUP, FAILD} OPERATION;

int op_reg(int request_id, int load, OPERATION op, int answer);

#endif
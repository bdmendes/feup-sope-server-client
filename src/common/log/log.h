#ifndef COMMON_LOG_LOGH
#define COMMON_LOG_LOGH

typedef enum {IWANT, RECVD, TSKEX, TSKDN, GOTRS, TOOLATE, CLOSD, GAVUP, FAILD} OPERATION;

void log_operation(OPERATION operation, int request_id, int load, int answer);

#endif
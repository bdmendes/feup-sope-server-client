#ifndef COMMON_LOG_LOGH
#define COMMON_LOG_LOGH

/**
 * @brief Enum with different types of message possible.
 */
typedef enum {IWANT, RECVD, TSKEX, TSKDN, GOTRS, TOOLATE, CLOSD, GAVUP, FAILD} OPERATION;

/**
 * @brief Function to register one operation
 * @param operation Operation to register
 * @param request_id Request identification
 * @param load Load of the operation 
 * @param answer Answer of the operation
 */
void log_operation(OPERATION operation, int request_id, int load, int answer);

#endif
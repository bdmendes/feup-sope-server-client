#ifndef MESSAGE_H
#define MESSAGE_H

#include <sys/types.h>

/**
 * @brief Structure to store a message information
 */
typedef struct {
    /** request id */
    int rid; 
    /** process id */      
    pid_t pid;    
    /** thread id */
    pthread_t tid; 
    /** task load */
    int tskload;  
    /** task result */
    int tskres;    
} Message;

/**
 * @brief Function to build a message
 * @param message Pointer for return the assembled message
 * @param request_id Resquest id of the 
 * @param load
 * @param answer
 */
void assemble_message(Message *message, int request_id, int load, int answer);

#endif
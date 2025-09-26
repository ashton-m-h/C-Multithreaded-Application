/**
 * Struct that should be passed as an argument to client_thread
 * function.
 *
 * Containst the following information:
 *
 * fd: file descriptor
 * maxSize: max size allowed for a file in bytes
 * tmpFileSem: semaphore protecting the temporary file
 * clntLim: semaphore limiting total number of clients
 *
 * sighupData: pointer to the statistics
 * sighupSem: semaphore protecting writes to the statistics
 *
 */

#ifndef CLIENTARG_H
#define CLIENTARG_H

#include <stdint.h>
#include <semaphore.h>
#include "sighup_struct.h"

typedef struct {
    int fd;
    uint32_t maxSize;
    sem_t* tmpFileSem;
    sem_t* clntLim;
    SighupData* sighupData;
    sem_t* sighupSem;
} ClientArg;

#endif

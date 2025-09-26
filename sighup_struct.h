/**
 * * Struct containing all information that will be displayed
 * to stderr when SIGHUP is sent to the process
 *
 * numClients: clients currently connected
 * clntsCompleted: all clients that total number of clients that
 *      connected and disconnected since program start
 * detectReqs: Number of detection requests responded to successfully
 * replaceReqs: Number of replacement requests responded to successfully
 * badReqs: Number of badly formed requests
 * sighupSem: sem protecting these values, as we only want to read
 * when no one is trying to write to it
 */

#ifndef SIGHUP_STRUCT_H
#define SIGHUP_STRUCT_H

#include <stdint.h>
#include <semaphore.h>

typedef struct {
    uint32_t numClients;
    uint32_t clntsCompleted;
    uint32_t detectReqs;
    uint32_t replaceReqs;
    uint32_t badReqs;
    sem_t* sighupSem;
} SighupData;

#endif

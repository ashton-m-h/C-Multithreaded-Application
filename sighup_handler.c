#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include "sighup_struct.h"

void print_stats(SighupData* sighupData);

/**
 * Thread function for the sighup handling thread
 * blocks waiting for SIGHUP, and then prints the data
 * runs in a loop
 * arg must be SighupData
 */
void* sighup_thread(void* arg)
{
    SighupData* sighupData = (SighupData*)arg;

    // Unblock SIGHUP for this thread
    sigset_t set;
    int sig;
    sigemptyset(&set);
    sigaddset(&set, SIGHUP);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    while (1) {
        sigwait(&set, &sig); // Block until system comes
        // Prevent race conditions, make sure only read
        // when no one's trying to write to
        sem_wait(sighupData->sighupSem);
        print_stats(sighupData);
        sem_post(sighupData->sighupSem);
    }

    return NULL;
}

/**
 * Prints the statistics held in the strcut sighupData to stderr
 * sighupData: pointer to struct containing the statistics
 */
void print_stats(SighupData* sighupData)
{
    const char* const numClientsStr = "Num clients connected: ";
    const char* const clntsCompletedStr = "Clients completed: ";
    const char* const detectReqsStr = "Face detect requests: ";
    const char* const replaceReqsStr = "Face replacement requests: ";
    const char* const badReqsStr = "Bad requests: ";

    fprintf(stderr, "%s%d\n", numClientsStr, sighupData->numClients);
    fprintf(stderr, "%s%d\n", clntsCompletedStr, sighupData->clntsCompleted);
    fprintf(stderr, "%s%d\n", detectReqsStr, sighupData->detectReqs);
    fprintf(stderr, "%s%d\n", replaceReqsStr, sighupData->replaceReqs);
    fprintf(stderr, "%s%d\n", badReqsStr, sighupData->badReqs);

    fflush(stderr);

    return;
}

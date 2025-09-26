#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <semaphore.h>
#include <signal.h>
#include "client_arg.h"
#include "cascade_struct.h"
#include "image_protocol.h"
#include "sighup_struct.h"

#define REPLACE 1
#define DETECT 0

void* client_thread(void* arg);
int receive_serv(FILE* in, FILE* output, uint32_t maxSize, sem_t* tmpFileSem,
        CvCascs* cvCascs, SighupData* sd, sem_t* shDataSem, int* reqType);
void send_error(char* errMsg, FILE* output);
int process_image(CvCascs* cvCascs, sem_t* tmpFileSem);
ImageProt construct_imageprot_serv(void);
void write_stream(ImageProt imageProt, FILE* to);
void write_stream_nc(ImageProt imageProt, FILE* to);
void handle_statistics(SighupData* sighupData, sem_t* sighupSem);
void* sighup_thread(void* arg);
void spawn_thread(sem_t* clntLim, struct sockaddr_in* fromAddr,
        socklen_t* fromAddrSize, SighupData* sighupData, sem_t* sighupSem,
        uint32_t maxImageSize, sem_t* tmpFileSem, int fdServer);
void release_cvcascs(CvCascs* cvCascs);
void close_free_post(CvCascs* cvCascs, FILE* from, sem_t* clntLim);

/**
 * Processes the connections for a server and
 * spawns a thread to handle the client. Then detaches
 * the thread Id and loops
 *
 * Also blocks SIGHUP for all threads spawned
 * as the sighup handling thread will unblock it
 *
 * Passes maxImageSize into each client handling thread
 * initiates a semaphore blocking a new client from spawning
 * until there is enough with clientLim
 */
void process_connections(int fdServer, uint32_t maxImageSize, int clientLim)
{
    struct sockaddr_in fromAddr;
    socklen_t fromAddrSize;
    sem_t* tmpFileSem = malloc(sizeof(sem_t));
    sem_t* clntLim = malloc(sizeof(sem_t));
    sem_t* sighupSem = malloc(sizeof(sem_t));
    sem_init(tmpFileSem, 0, 1); // Only 1 thread can access tmpFile
    sem_init(clntLim, 0, clientLim); // Limits total clients

    // Initiate sighup data to 0 and init the sighup protector sem
    SighupData* sighupData = calloc(1, sizeof(SighupData));
    sem_init(sighupSem, 0, 1);
    sigset_t set; // To set the signal
    sigemptyset(&set); // Clear it
    sigaddset(&set, SIGHUP); // Add SIGHUP to it
    sigaddset(&set, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
    sighupData->sighupSem = sighupSem;

    // Now spawn the SIGHUP handling thread
    pthread_t sighupID;
    pthread_create(&sighupID, NULL, sighup_thread, sighupData);
    pthread_detach(sighupID);

    while (1) {
        spawn_thread(clntLim, &fromAddr, &fromAddrSize, sighupData, sighupSem,
                maxImageSize, tmpFileSem, fdServer);
    }
    free(sighupData);
}

/**
 * This is the client thread function.
 * Opens the file descriptor, then reads incoming message,
 * depending on the file type will process the image
 * then write back to the file descriptor, closes the streams
 * and returns
 * Must take the ClientArg* as an argument
 */
void* client_thread(void* arg)
{
    ClientArg clientArg = *(ClientArg*)arg;
    free(arg); // Free the malloced arg
    // Open 2 files streams, making sure they are looking at
    // separate file descriptors
    FILE* from = fdopen(clientArg.fd, "r");
    FILE* to = fdopen(dup(clientArg.fd), "w");
    CvCascs* cvCascs;
    int errNum = 0;

    while (!feof(from)) {
        cvCascs = calloc(1, sizeof(CvCascs));
        int reqType; // Keeping track for the statistics
        if (receive_serv(from, to, clientArg.maxSize, clientArg.tmpFileSem,
                    cvCascs, clientArg.sighupData, clientArg.sighupSem,
                    &reqType)) {
            close_free_post(cvCascs, from, clientArg.clntLim);
            handle_statistics(clientArg.sighupData, clientArg.sighupSem);
            return NULL;
        }

        // Load the temporary files
        errNum = process_image(cvCascs, clientArg.tmpFileSem);
        if (errNum) {
            char* noFacesErrMsg = "no faces detected in image";
            send_error(noFacesErrMsg, to);
            close_free_post(cvCascs, from, clientArg.clntLim);
            handle_statistics(clientArg.sighupData, clientArg.sighupSem);
            return NULL;
        }
        // Construct and send the data
        ImageProt sendImageProt = construct_imageprot_serv();
        write_stream_nc(sendImageProt, to); // Does not close stream
        sem_post(clientArg.tmpFileSem); // Waited in construct image
        sem_wait(clientArg.sighupSem);
        if (reqType == REPLACE) {
            (clientArg.sighupData)->replaceReqs++;
        } else {
            (clientArg.sighupData)->detectReqs++;
        }
        sem_post(clientArg.sighupSem);
    }
    // Close streams and free memoery
    fclose(to);
    close_free_post(cvCascs, from, clientArg.clntLim);
    handle_statistics(clientArg.sighupData, clientArg.sighupSem);

    return NULL;
}

/**
 * Helper function for the server
 * frees cvCascs (and releases images if needs to)
 * and fcloses from
 * and posts clntLim
 * DOES NOT FCLOSE WRITE STREAM, THIS NEEDS TO BE DONE SEPARATELY
 */
void close_free_post(CvCascs* cvCascs, FILE* from, sem_t* clntLim)
{
    fclose(from);
    if (cvCascs->faceCascade) { // Relase if not yet released
        release_cvcascs(cvCascs);
    }
    sem_post(clntLim);
    free(cvCascs);

    return;
}

/**
 * Helper function for incrementing the statistics when
 * sighup is called
 * This function should be invoked before the client thread exits
 * DOES NOt increment detect/replace request
 */
void handle_statistics(SighupData* sighupData, sem_t* sighupSem)
{
    sem_wait(sighupSem);
    sighupData->numClients--;
    sighupData->clntsCompleted++;
    sem_post(sighupSem);

    return;
}

/**
 * Helper function for the thread spawning loop.
 * Constructs a clientArg struct and spawns a thread
 * with the correct info
 * Also protects info with the semaphore
 *
 * clntLim: semaphore stopping more than the most amount of clntLims
 * from spawning (Is posted inside the spawned thread)
 * fd: port fd
 * fromAddr: sockaddr_in* for the from addr
 * fromAddrSize: Needed for accept
 * sighupData: pointer to the statistics struct
 * sighupSem: semaphore protecting the sighupData
 * maxImageSize: use specified max amount of bytes for image
 * tmpFileSem: semaphore protecting the temporary file
 * fdServer: File descriptor for the server
 */
void spawn_thread(sem_t* clntLim, struct sockaddr_in* fromAddr,
        socklen_t* fromAddrSize, SighupData* sighupData, sem_t* sighupSem,
        uint32_t maxImageSize, sem_t* tmpFileSem, int fdServer)
{
    int fd; // File descriptor for the client
    sem_wait(clntLim);
    *fromAddrSize = sizeof(struct sockaddr_in);
    fd = accept(fdServer, (struct sockaddr*)fromAddr, fromAddrSize);
    if (fd < 0) {
        sem_post(clntLim);
        return;
    }

    // Construct clientArg
    ClientArg* clientArg = malloc(sizeof(ClientArg));
    clientArg->fd = fd;
    clientArg->maxSize = maxImageSize;
    clientArg->tmpFileSem = tmpFileSem;
    clientArg->clntLim = clntLim;
    clientArg->sighupData = sighupData;
    clientArg->sighupSem = sighupSem;

    // Increment num clients
    sem_wait(sighupSem);
    sighupData->numClients++;
    sem_post(sighupSem);
    // Spawn thread
    pthread_t threadID;
    pthread_create(&threadID, NULL, client_thread, clientArg);
    pthread_detach(threadID);

    return;
}

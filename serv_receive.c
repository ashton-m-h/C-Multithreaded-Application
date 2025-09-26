#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/objdetect/objdetect_c.h>
#include "image_protocol.h"
#include "cascade_struct.h"
#include "sighup_struct.h"

#define PREFIX 0x23107231
#define ERR_OPTYPE 3
#define EXPECT_RUNNINGTOTAL 2
#define REPLACE 1
#define DETECT 0
#define BADLY_FORMED_REQUEST 2

void send_error(char* errMsg, FILE* output);
void write_stream(ImageProt imageProt, FILE* to);
int receive_image(FILE* stream, uint8_t** data, int* dataSize);
int construct_vars(ImageProt* imgP, FILE* in, FILE* out, uint32_t maxSize);
int init_cascades(CvCascs* cvCascs);
int load_replace(CvCascs* cvCascs);
void release_cvcascs(CvCascs* cvCascs);
void free_and_release(ImageProt* imageProt, CvCascs* cvCascs);

/**
 * Server function for receiving data from the file stream
 * and then outputs the corresponding data
 *
 * in/out: in/out file streams
 * maxSize: max bytes allowed for the image
 * tmpFileSem: semaphore protecing the temporary file
 * cvCascs: loads the received files into it
 *
 * calls the cascade function, and closes stream
 * returns 1 on failure, 0 on success
 *
 * sighupData: increment the badReqs if badly formed request
 * protect the write with sighupSem
 *
 * reqType: Changes this value depending on the request type
 * Cant change the stats here because there's still a chance
 * the request won't be successful
 */
int receive_serv(FILE* in, FILE* out, uint32_t maxSize, sem_t* tmpFileSem,
        CvCascs* cvCascs, SighupData* sighupData, sem_t* sighupSem,
        int* reqType)
{
    char* cvLoadErrMsg = "invalid image";
    // Declare variables ready to be received
    ImageProt recImageProt = {0};
    int errNum = 0;
    if ((errNum = construct_vars(&recImageProt, in, out, maxSize))) {
        if (errNum == BADLY_FORMED_REQUEST) { // For statistics
            sem_wait(sighupSem);
            sighupData->badReqs++;
            sem_post(sighupSem);
        }
        return 1; // Failed
    }
    sem_wait(tmpFileSem); // Protect the temporary file

    FILE* tmpFile; // File stream to temporary file
    tmpFile = fopen("/tmp/imagefile.jpg", "w");
    fwrite(recImageProt.img1, sizeof(uint8_t), recImageProt.img1Size, tmpFile);
    fclose(tmpFile);
    if (!init_cascades(cvCascs)) {
        send_error(cvLoadErrMsg, out);
        free_and_release(&recImageProt, cvCascs);
        sem_post(tmpFileSem);
        return 1;
    }
    if (recImageProt.opType == REPLACE) { // Truncate and rewrite with replace
        tmpFile = fopen("/tmp/imagefile.jpg", "w");
        fwrite(recImageProt.img2, sizeof(uint8_t), recImageProt.img2Size,
                tmpFile);
        fclose(tmpFile);
        if (!load_replace(cvCascs)) {
            send_error(cvLoadErrMsg, out); // Send error and free mem
            free_and_release(&recImageProt, cvCascs);
            release_cvcascs(cvCascs);
            sem_post(tmpFileSem);
            return 1;
        }
    }

    free(recImageProt.img1);
    if (recImageProt.img2) {
        free(recImageProt.img2);
    }
    sem_post(tmpFileSem);
    if (recImageProt.opType == REPLACE) { // Recored the opType for stats
        *reqType = REPLACE;
    } else {
        *reqType = DETECT;
    }
    return 0;
}

/**
 * Sends an error in the communication protocol
 * back through the File stream with the given
 * string, uses the ImageProt struct
 *
 * errMsg: message sent to client
 * output: FILE stream going to client
 */
void send_error(char* errMsg, FILE* output)
{
    // Construct imageProt
    ImageProt imageProt = {0};
    imageProt.prefix = PREFIX;
    imageProt.opType = ERR_OPTYPE;
    uint32_t msgLength = strlen(errMsg);
    imageProt.img1Size = msgLength;
    uint8_t* msg = calloc(msgLength, sizeof(uint8_t));
    for (int i = 0; i < (int)msgLength; ++i) {
        msg[i] = (uint8_t)errMsg[i];
    }
    imageProt.img1 = msg;

    write_stream(imageProt, output); // Frees msg1 and fcloses output
    return;
}

/**
 * Helper function for freeing the data in imageProt
 * and releasing the images held in cvCascs
 */
void free_and_release(ImageProt* imageProt, CvCascs* cvCascs)
{
    if (imageProt->img1) {
        free(imageProt->img1);
    }
    if (imageProt->img2) {
        free(imageProt->img2);
    }
    release_cvcascs(cvCascs);
}

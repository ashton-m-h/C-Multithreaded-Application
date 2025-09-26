#include <stdio.h>
#include <stdlib.h>
#include "image_protocol.h"
#define PREFIX 0x23107231
#define EXP_RUNTOTAL 3
#define REPLACE 1
#define DETECT 0
#define BADLY_FORMED_REQUEST 2
int receive_and_check_file(
        ImageProt* imageProt, FILE* in, FILE* out, int imgNo);
void send_error(char* errMsg, FILE* output);
void send_badrequest(FILE* to);
int check_img_size(
        ImageProt* recImageProt, FILE* out, uint32_t maxSize, int imageNo);

/**
 * Server function that receives data over the
 * file stream in, and then constructs recImageProt
 * with the data.
 *
 * Sends an error back through file stream IN and
 * frees memory
 *
 * recImageProt: fills this struct with information incoming from
 * file stream IN.
 * in: File stream coming into program
 * out: Sends an error message to this file stream if error
 * maxSize: the maxSize that is allowed for the image
 *
 *
 * returns 0 on success, 1 on error, and 2 if badly formed request
 */
int construct_vars(
        ImageProt* recImageProt, FILE* in, FILE* out, uint32_t maxSize)
{
    char* commErr = "invalid message";
    char* invOpType = "invalid operation type";

    int runningTotal = 0; // Read Prefix, optype and img1Size:
    runningTotal += fread(&(recImageProt->prefix), sizeof(uint32_t), 1, in);
    if (runningTotal != 1) { // If nothing to read, return commErr
        send_error(commErr, out);
        return 1;
    }
    // If we got something but it's not the right prefix, send_badrequest
    if (recImageProt->prefix != PREFIX) {
        send_badrequest(out);
        return BADLY_FORMED_REQUEST;
    }
    runningTotal += fread(&(recImageProt->opType), sizeof(uint8_t), 1, in);
    runningTotal += fread(&(recImageProt->img1Size), sizeof(uint32_t), 1, in);
    if (runningTotal != EXP_RUNTOTAL) {
        send_error(commErr, out);
        return 1;
    }
    if (check_img_size(recImageProt, out, maxSize, 1)) {
        return 1;
    }
    if (recImageProt->opType != REPLACE && recImageProt->opType != DETECT) {
        send_error(invOpType, out);
        return 1;
    }
    if (receive_and_check_file(recImageProt, in, out, 1)) {
        return 1;
    }
    if (recImageProt->opType == REPLACE) {
        if (fread(&(recImageProt->img2Size), sizeof(uint32_t), 1, in) != 1) {
            send_error(commErr, out);
            free(recImageProt->img1);
            return 1;
        }
        if (check_img_size(recImageProt, out, maxSize, 2)) {
            free(recImageProt->img1);
            return 1;
        }
        if (receive_and_check_file(recImageProt, in, out, 2)) {
            return 1;
        }
    }
    return 0;
}

/**
 * Helper function to check the image size, if it's 0 bytes
 * send 0byteserror to client if it is more than the maxSize send the
 * more than maxSize error message
 * If imageNo is set to 1: check detect img
 * if imageNo is set to 2: check replace img
 * imageNo MUST BE 1 or 2
 * recImageProt: image protocol received from client
 * out: file stream going to client (send error to this guy)
 * maxSize: max size in bytes of image
 *
 * returns 0 on success, 1 on error
 */
int check_img_size(
        ImageProt* recImageProt, FILE* out, uint32_t maxSize, int imageNo)
{
    char* zeroBytesErr = "image is 0 bytes";
    char* tooLargeErr = "image too large";

    if (imageNo == 1) { // Check img 1
        if (recImageProt->img1Size > maxSize) {
            send_error(tooLargeErr, out);
            return 1;
        }
        if (recImageProt->img1Size == 0) {
            send_error(zeroBytesErr, out);
            return 1;
        }
    } else if (imageNo == 2) { // Check img 2
        if (recImageProt->img2Size > maxSize) {
            send_error(tooLargeErr, out);
            return 1;
        }
        if (recImageProt->img2Size == 0) {
            send_error(zeroBytesErr, out);
            return 1;
        }
    }

    return 0;
}

/**
 * Helper function to read and input file, checking correct number
 * of bytes are read, assumes the img size is already loaded
 * imgNo decides wehter to load into img 1 or img 2, also frees mem
 *
 * imageProt: image protocol (read from client)
 * in : file stream receiving info from client
 * out : File stream sending information to the client
 * imgNo: should be 1 if checking face detect,
 * 2 if checking face replace
 */
int receive_and_check_file(ImageProt* imageProt, FILE* in, FILE* out, int imgNo)
{
    char* commErr = "invalid message";
    if (imgNo == 1) {
        imageProt->img1 = malloc(sizeof(uint8_t) * imageProt->img1Size);
        if (fread(imageProt->img1, sizeof(uint8_t), imageProt->img1Size, in)
                != imageProt->img1Size) {
            free(imageProt->img1);
            send_error(commErr, out);
            return 1;
        }
    } else if (imgNo == 2) {
        imageProt->img2 = malloc(sizeof(uint8_t) * imageProt->img2Size);
        if (fread(imageProt->img2, sizeof(uint8_t), imageProt->img2Size, in)
                != imageProt->img2Size) {
            free(imageProt->img1);
            free(imageProt->img2);
            send_error(commErr, out);
            return 1;
        }
    }
    return 0;
}

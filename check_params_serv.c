#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include "cmd_line_params_serv.h"
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/objdetect/objdetect_c.h>

#define USAGE_ERR_CODE 3
#define TMPFILE_ERR_CODE 14
#define PORT_ERR_CODE 1
#define CASCADE_ERR_CODE 11

#define DEFAULT_CLNTLIM 10000
#define CLNTLIM_MAX 10000
#define DEFAULT_MAXSIZE 4294967295 // 2**32 - 1
#define MAX_MAXSIZE 4294967295
#define EPH_PORT "39655"

int load_cascades(CvHaarClassifierCascade** faceCascade,
        CvHaarClassifierCascade** eyesCascade);

int open_listen(const char* port);

/**
 * Function that checks the parameters given in the initial input.
 * Checks bounds for the given params and sets a value if set to 0
 * Sets value to pointed to by portNo to the opened port
 *
 * Also tries to create and truncate /jpg/imagefile.jpg, then closes it. Else
 * return error code
 * Also check that two files specified in the resources folder
 * can be opened
 *
 * params: Params that the user has specified on the cmd line
 * errArg: Fill this pointer with an argument for throwing error
 * portNo: Fill this with the file descriptor from opening the port
 *
 * Assumes that parms has all parameters made except for portNum
 */
int check_serv_params(CmdLineParamsServ* params, char** errArg, int* portNo)
{
    if (params->clientLim > CLNTLIM_MAX) {
        return USAGE_ERR_CODE;
    }
    if (params->clientLim == 0) {
        params->clientLim = DEFAULT_CLNTLIM;
    }

    if (params->maxSize > (uint32_t)MAX_MAXSIZE) {
        return USAGE_ERR_CODE;
    }
    if (params->maxSize == 0) {
        params->maxSize = DEFAULT_MAXSIZE;
    }

    // Temporary image file checking
    FILE* tmpFile;
    if (!(tmpFile = fopen("/tmp/imagefile.jpg", "w"))) {
        return TMPFILE_ERR_CODE;
    }
    fclose(tmpFile);

    // Check the cascade classifiers can be loaded
    CvHaarClassifierCascade* faceCascade;
    CvHaarClassifierCascade* eyesCascade;
    if (load_cascades(&faceCascade, &eyesCascade)) {
        return CASCADE_ERR_CODE;
    }
    cvReleaseHaarClassifierCascade(&eyesCascade);
    cvReleaseHaarClassifierCascade(&faceCascade);

    // Port checking
    int fdport;
    if ((fdport = open_listen(params->portNum)) == PORT_ERR_CODE) {
        (*errArg) = strdup(params->portNum);
        return PORT_ERR_CODE;
    }

    *portNo = fdport;
    return 0;
}

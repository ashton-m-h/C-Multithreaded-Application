/**
 * Struct for holding information gathered from the
 * command line when launching uqfaceclient
 * portNum: (required) is a char* since it is possible for the user to
 * specify a string in the command line for the portnum
 * detectFilename: (optional) , should contain data for the image
 * to b sent to uq facedetect
 * replaceFilename: (optional) , image sent to the server that
 * replaces faces with
 * outputFilename: (optional) , filename for the process
 * image to be saved to.
 */

#ifndef CMDLINEPARAMSCLIENT_H
#define CMDLINEPARAMSCLIENT_H

typedef struct {
    char* portNum;
    char* detectFilename;
    char* replaceFilename;
    char* outputFilename;
} CmdLineParamsClient;

#endif

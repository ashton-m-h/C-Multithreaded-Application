#include <stdio.h>
#include <stdlib.h>

#define USG_ERR_CODE 3
#define TMPFILE_ERR_CODE 14
#define PORT_ERR_CODE 1
#define CASC_ERR_CODE 11

/**
 * Error handling function for the serv
 * Switches the errCode, and prints the corresponding
 * string to stderr (with errArg if needs to)
 * then exits program with the specified errCode
 */
void exit_error(int errCode, char* errArg)
{
    const char* usgErr
            = "Usage: ./uqfacedetect clientlimit maxsize [portnum]\n";
    const char* tmpFileErr
            = "uqfacedetect: cannot open image file for writing\n";

    const char* portErr[2]
            = {"uqfacedetect: cannot listen on given port \"", "\"\n"};

    const char* cascErr = "uqfacedetect: unable to load a cascade classifier\n";

    switch (errCode) {
    case USG_ERR_CODE:
        fprintf(stderr, usgErr);
        break;
    case TMPFILE_ERR_CODE:
        fprintf(stderr, tmpFileErr);
        break;
    case PORT_ERR_CODE:
        fprintf(stderr, "%s%s%s", portErr[0], errArg, portErr[1]);
        break;
    case CASC_ERR_CODE:
        fprintf(stderr, cascErr);
        break;
    default:
        fprintf(stderr, "General Error with arg: %s\n", errArg);
    }

    if (errArg) {
        free(errArg);
    }

    exit(errCode);
}

#include <stdio.h>
#include <stdlib.h>

#define USG_ERR_CODE 17
#define READ_ERR 10
#define WRITE_ERR 3
#define PORT_ERR 18
#define COMM_ERR 16
#define SERV_ERR 19

/**
 * Error handling function,
 * utilises a switch case to print
 * the corresponding error with the correct
 * error message to stderr, as well as
 * fprintf the errArg if
 * the error requires
 *
 * returns nothing, exits program instead with
 * errCode
 */
void exit_error(int errCode, char* errArg)
{
    const char* usgErr
            = "Usage: ./uqfaceclient portnumber [--detect filename] "
              "[--replacefile filename] [--outputfilename filename]\n";
    const char* readErr[2] = {
            "uqfaceclient: cannot open the input file \"", "\" for reading\n"};
    const char* writeErr[2] = {
            "uqfaceclient: cannot open the output file \"", "\" for writing\n"};

    const char* portErr[2] = {
            "uqfaceclient: unable to connect to the server on port \"", "\"\n"};

    const char* commErr = "uqfaceclient: unexpected communication error\n";

    const char* servErr[2] = {
            "uqfaceclient: received the following error message: \"", "\"\n"};

    switch (errCode) {
    case USG_ERR_CODE:
        fprintf(stderr, usgErr);
        break;
    case READ_ERR:
        fprintf(stderr, "%s%s%s", readErr[0], errArg, readErr[1]);
        break;
    case WRITE_ERR:
        fprintf(stderr, "%s%s%s", writeErr[0], errArg, writeErr[1]);
        break;
    case PORT_ERR:
        fprintf(stderr, "%s%s%s", portErr[0], errArg, portErr[1]);
        break;
    case COMM_ERR:
        fprintf(stderr, commErr);
        break;
    case SERV_ERR:
        fprintf(stderr, "%s%s%s", servErr[0], errArg, servErr[1]);
        break;
    default:
        fprintf(stderr, "General Error with arg: %s\n", errArg);
    }

    if (errArg) {
        free(errArg);
    }
    exit(errCode);
}

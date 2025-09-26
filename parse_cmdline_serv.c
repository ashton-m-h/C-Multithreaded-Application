#include "cmd_line_params_serv.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#define USAGE_ERR_CODE 3
#define NUM_ARGS_REQ_1 3
#define NUM_ARGS_REQ_2 4
#define PORTNUM_IND 3
#define MAXSIZE_IND 2
#define CLIENTLIM_IND 1

/**
 * Parses command line and fills the given options into
 * the pointer to the struct CmdLineParamsServ params
 * Returns 0 on success, else returns the usage error code
 * implying the user did not follow the specifications given
 * in the spec sheet. Does NOT free memory
 * Checks that clientLim and maxSize are proper numbers
 *
 * argc: argc from main (- first arg)
 * argv: argv from main (- first arg)
 * params: Fill this struct with the specified info
 */

int parse_cmdline_serv(int argc, char** argv, CmdLineParamsServ* params)
{
    if (argc != NUM_ARGS_REQ_1 && argc != NUM_ARGS_REQ_2) { // Either 3 or 4
        return USAGE_ERR_CODE;
    }

    // Check there are no empty arguments
    for (int i = 0; i < argc; ++i) {
        if (!strlen(argv[i])) {
            return USAGE_ERR_CODE;
        }
    }

    char* clientLimStr = strdup(argv[CLIENTLIM_IND]);
    // Check that it's an integer
    for (int i = 0; i < (int)strlen(clientLimStr); ++i) {
        if (i == 0) { // First character may be a plus
            if (!isdigit(clientLimStr[i]) && clientLimStr[i] != '+') {
                free(clientLimStr);
                return USAGE_ERR_CODE;
            }
        } else if (!isdigit(clientLimStr[i])) {
            free(clientLimStr);
            return USAGE_ERR_CODE;
        }
    }

    params->clientLim = atoi(clientLimStr);
    free(clientLimStr);

    char* maxSizeStr = strdup(argv[MAXSIZE_IND]);
    // Check that it's an integer
    for (int i = 0; i < (int)strlen(maxSizeStr); ++i) {
        if (!isdigit(maxSizeStr[i])) {
            free(maxSizeStr);
            return USAGE_ERR_CODE;
        }
    }

    params->maxSize = atoi(maxSizeStr);
    free(maxSizeStr);

    if (argc == NUM_ARGS_REQ_2) { // Has the portnum been specified?
        params->portNum = strdup(argv[PORTNUM_IND]);
    } else {
        params->portNum = strdup("0");
    }

    return 0; // Return 0 for success
}

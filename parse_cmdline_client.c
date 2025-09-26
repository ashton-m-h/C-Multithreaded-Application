#include "cmd_line_params_client.h"
#include <string.h>

#define USG_ERR_CODE 17
#define DETECT "--detect"
#define REPLACE "--replacefile"
#define OUTPUT "--outputfilename"

/**
 * Parses command line, filling in params with what the user provides
 * returns 0 on success, or the error code if there is an error
 * argc and argv should be identical to main
 * params is the struct we're filling with the users parameters
 * requires params to initially set to {0} before the function is called
 */
int parse_cmdline_client(int argc, char** argv, CmdLineParamsClient* params)
{
    // If no arguments -> usage error
    if (argc < 2) {
        return USG_ERR_CODE;
    }
    // Skip the first one as it's just the program name
    argc--;
    argv++;
    // Fisrt argument is always the portNum, throw error if empty
    if (!strlen(argv[0])) {
        return USG_ERR_CODE;
    }
    params->portNum = strdup(argv[0]);
    argc--;
    argv++;

    // Loop through the commad lines, make sure to check it's not empty string
    while (argc) {
        if (!strcmp(argv[0], DETECT) && argc > 1 && !params->detectFilename
                && strlen(argv[1])) {
            params->detectFilename = strdup((++argv)[0]);
            ++argv;
            argc -= 2;
        } else if (!strcmp(argv[0], REPLACE) && argc > 1
                && !params->replaceFilename && strlen(argv[1])) {
            params->replaceFilename = strdup((++argv)[0]);
            ++argv;
            argc -= 2;
        } else if (!strcmp(argv[0], OUTPUT) && argc > 1
                && !params->outputFilename && strlen(argv[1])) {
            params->outputFilename = strdup((++argv)[0]);
            ++argv;
            argc -= 2;
        } else {
            return USG_ERR_CODE;
        }
    }

    return 0;
}

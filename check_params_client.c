#include <stdio.h>
#include <string.h>
#include "cmd_line_params_client.h"
#include "file_struct.h"

#define READ_ERR 10
#define WRITE_ERR 3

/**
 * Function that checks the params inside params, and if files
 * cannot be opened, returns the corresponding error code
 * params: struct filled with the strings of the filenames,
 * given by the user
 * files: struct holding the output, detect and replace file
 * If no detect file specified, read from stdin
 * If no output file specified, write to stdout
 *
 * params: Checks the data in this struct
 * files: Opens the files and places the file streams into this struct
 * errArg: If returning an error number, fill this character with the error
 * Argument
 *
 * Also checks portNum to see if it can be opened
 */
int check_client_params(
        CmdLineParamsClient* params, FileStruct* files, char** errArg)
{
    if (params->detectFilename) {
        files->detectFile = fopen(params->detectFilename, "r");
        if (!files->detectFile) {
            *errArg = strdup(params->detectFilename);
            return READ_ERR;
        }
    } else {
        files->detectFile = stdin;
    }

    if (params->replaceFilename) {
        files->replaceFile = fopen(params->replaceFilename, "r");
        if (!files->replaceFile) {
            *errArg = strdup(params->replaceFilename);
            return READ_ERR;
        }
    }

    if (params->outputFilename) {
        files->outputFile = fopen(params->outputFilename, "w");
        if (!files->outputFile) {
            *errArg = strdup(params->outputFilename);
            return WRITE_ERR;
        }
    } else {
        files->outputFile = stdout;
    }

    return 0;
}

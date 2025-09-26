#include <stdio.h>
#include <stdlib.h>
#include "cmd_line_params_client.h"
#include "file_struct.h"

/**
 * Function that frees all memory contained in these
 * structs. Can pass NULL in as it checks before freeing
 * params: Frees the duplicated strings in this struct
 * files: fcloses the opened file streams in this struct
 */

void clean_all(CmdLineParamsClient* params, FileStruct* files)
{
    if (params) {
        if (params->detectFilename) {
            free(params->detectFilename);
        }
        if (params->replaceFilename) {
            free(params->replaceFilename);
        }
        if (params->outputFilename) {
            free(params->outputFilename);
        }
        if (params->portNum) {
            free(params->portNum);
        }
    }

    if (files) {
        if (files->replaceFile) {
            fclose(files->replaceFile);
        }
        if (files->detectFile) {
            fclose(files->detectFile);
        }
        if (files->outputFile) {
            fclose(files->outputFile);
        }
    }

    return;
}

#include <stdlib.h>
#include "cmd_line_params_serv.h"

/**
 * Frees all memory contained in the structs if it exists.
 */

void clean_all(CmdLineParamsServ* params)
{
    if (params) {
        if (params->portNum) {
            free(params->portNum);
        }
    }
}

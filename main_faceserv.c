#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "cmd_line_params_serv.h"

int parse_cmdline_serv(int argc, char** argv, CmdLineParamsServ* params);
void exit_error(int errCode, char* errArg);
void clean_all(CmdLineParamsServ* params);
int check_serv_params(CmdLineParamsServ* params, char** erArg, int* portNo);
void process_connections(int fdServer, uint32_t maxSize, int clientLim);

/**
 * Main function for the server
 * 1. Parses Command line
 * 2. Checks command line
 * 3. Runs the process connections loop
 */
int main(int argc, char** argv)
{
    CmdLineParamsServ params = {0};

    int errCode;
    char* errArg = NULL;
    if ((errCode = parse_cmdline_serv(argc, argv, &params))) {
        clean_all(&params);
        exit_error(errCode, NULL);
    }

    int portNo;
    if ((errCode = check_serv_params(&params, &errArg, &portNo))) {
        clean_all(&params);
        exit_error(errCode, errArg);
    }
    free(params.portNum);
    process_connections(portNo, (uint32_t)params.maxSize, params.clientLim);
    close(portNo);

    clean_all(&params);
    return 0;
}

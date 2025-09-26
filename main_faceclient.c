#include <stdio.h>
#include <stdlib.h>
#include "cmd_line_params_client.h"
#include "file_struct.h"
#include "image_protocol.h"

int parse_cmdline_client(int argc, char** argv, CmdLineParamsClient* params);
void exit_error(int errCode, char* errArg);
int check_client_params(
        CmdLineParamsClient* params, FileStruct* files, char** errArg);
void clean_all(CmdLineParamsClient* params, FileStruct* files);
int open_port(char* portNum, char** errArg, FILE** to, FILE** from);
ImageProt construct_request(FileStruct* files);
void write_stream(ImageProt imageProt, FILE* to);
int receive_clnt(FILE* in, char** errArg, FILE* output);

/**
 * Main function for the client.
 * Does the following:
 * 1. parses command line
 * 2. Opens ports and files
 * 3. Consctruct imageProt send request
 * 4. Writes the request to the specified port
 * 5. Receives data back from port
 * 6. cleans all the files sand returns
 */
int main(int argc, char** argv)
{
    // Parse Command Line
    CmdLineParamsClient params = {0};
    int errNo;
    if ((errNo = parse_cmdline_client(argc, argv, &params))) {
        clean_all(&params, NULL);
        exit_error(errNo, NULL);
    }

    // Open port and files
    FileStruct files = {0};
    char* errArg = NULL;
    if ((errNo = check_client_params(&params, &files, &errArg))) {
        clean_all(&params, &files);
        exit_error(errNo, errArg);
    }

    FILE* out; // out to the server
    FILE* in; // in from the server
    if ((errNo = open_port(params.portNum, &errArg, &out, &in))) {
        clean_all(&params, &files);
        exit_error(errNo, errArg);
    }
    // Construct and send request to the server
    ImageProt sendRequest = construct_request(&files);
    write_stream(sendRequest, out);
    // Receive data from the server
    if ((errNo = receive_clnt(in, &errArg, files.outputFile))) {
        clean_all(&params, &files);
        exit_error(errNo, errArg);
    }

    clean_all(&params, &files);
    return 0;
}

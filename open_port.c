#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <netdb.h>

#define PORT_ERRNUM 18

/**
 * Function that tries to open a port given on the specified char*
 * (although it has to be an int).
 * Returns 0 if no error and fills FILE* to and FILE* from
 * with the corresponding FILE* going to and from the given
 * port
 * Otherwise, return the specified error number and fill char* errArg
 * with the corresponding error argument
 *
 * portNum: STRING that is trying to be opened as a port
 * errArg: fill this pointer with an argument for the error function
 * if returning error
 * to: fill this file stream with a stream going to the server
 * from: fill this file stream with a stream going back to the client
 */
int open_port(char* portNum, char** errArg, FILE** to, FILE** from)
{
    struct addrinfo* ai = 0;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int err;
    if ((err = getaddrinfo(NULL, portNum, &hints, &ai))) {
        freeaddrinfo(ai);
        *errArg = strdup(portNum);
        return PORT_ERRNUM;
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(fd, ai->ai_addr, sizeof(struct sockaddr))) {
        freeaddrinfo(ai);
        *errArg = strdup(portNum);
        return PORT_ERRNUM;
    }

    int fd2 = dup(fd);
    FILE* toP = fdopen(fd, "w");
    FILE* fromP = fdopen(fd2, "r");
    *to = toP;
    *from = fromP;
    freeaddrinfo(ai);

    return 0;
}

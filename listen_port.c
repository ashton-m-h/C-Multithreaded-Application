#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define ERR_CODE 1
#define QUEUE_LENGTH 10

/**
 *  Returns listening socket (or a failure code).
 *  Tries to listen on the given port
 *  Also prints the port number to stderr
 *
 *  port : Try to open this port
 */
int open_listen(const char* port)
{
    struct addrinfo* ai = 0;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // listen on all IP addresses

    int err;
    if ((err = getaddrinfo(NULL, port, &hints, &ai))) {
        freeaddrinfo(ai);
        return ERR_CODE;
    }

    // Create a socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0); // 0=default protocol (TCP)
    if (listenfd < 0) {
        freeaddrinfo(ai);
        return ERR_CODE;
    }

    // Allow address (port number) to be reused immediately
    int optVal = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(int))
            < 0) {
        freeaddrinfo(ai);
        return ERR_CODE;
    }
    if (bind(listenfd, ai->ai_addr, sizeof(struct sockaddr)) < 0) {
        freeaddrinfo(ai);
        return ERR_CODE;
    }
    if (listen(listenfd, QUEUE_LENGTH)
            < 0) { // Up to 10 connection requests can queue
        // (Reality on moss is that this queue length parameter is ignored)
        freeaddrinfo(ai);
        return ERR_CODE;
    }

    // Print the port number and flushes (net4.c)
    struct sockaddr_in ad;
    memset(&ad, 0, sizeof(struct sockaddr_in));
    socklen_t addrlen = sizeof(ad);
    getsockname(listenfd, (struct sockaddr*)&ad, &addrlen);
    fprintf(stderr, "%d\n", ntohs(ad.sin_port));
    fflush(stderr);
    freeaddrinfo(ai);
    return listenfd;
}

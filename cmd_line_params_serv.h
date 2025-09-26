/**
 * Struct for holding information gathered from the
 * command line when launging uqfacedetect
 *
 * clientLim: first commad line option
 * the max amount of clients that will be
 * handled simultaneously.
 * MANDATORY, must be integer between 0 and 10000
 * If 0 then no limit
 *
 * maxSize: second command line option
 * Specifies the maximum size of bytes that will be
 * accepted from clients.
 * must be int between 0 and 2**32 - 1.
 * MANDATORY, if 0 then set to 2**32 - 1
 * Must be uint32_t because large number
 *
 * portNum: The port number. If 0 or absent,
 * get a random one.
 */

#ifndef CMDLINEPARAMSSERV_H
#define CMDLINEPARAMSSERV_H

#include <stdint.h>

typedef struct {
    int clientLim;
    uint64_t maxSize;
    char* portNum;
} CmdLineParamsServ;

#endif

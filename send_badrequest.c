#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void read_stream(FILE* stream, uint32_t* size, uint8_t** data);

/**
 *
 * Function that sends the contents of:
 * /local/courses/csse2310/resources/a4/responsefile
 * to the client (Which should happen in the case of
 * an incorrect prefix
 *
 * to: file stream to the client
 * flushed and closes to
 *
 */

void send_badrequest(FILE* to)
{
    uint8_t* data;
    uint32_t dataSize;
    FILE* responseFile
            = fopen("/local/courses/csse2310/resources/a4/responsefile", "r");

    read_stream(responseFile, &dataSize, &data);

    // Close the response file
    fclose(responseFile);
    // Send data back to client
    fwrite(data, sizeof(uint8_t), dataSize, to);
    fflush(to);
    fclose(to);

    return;
}

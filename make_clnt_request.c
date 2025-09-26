#include "file_struct.h"
#include "image_protocol.h"
#include <stdint.h>
#include <stdlib.h>

#define PREFIX 0x23107231
#define REPLACEMENT_REQUEST 1
#define DETECTION_REQUEST 0

void read_stream(FILE* stream, uint32_t* size, uint8_t** data);

/**
 * Constructing the specified protocol, taking data from
 * files given in FileStruct files and then storing the data and returning
 * it in a ImageProt
 * Returns a struct ImageProt which holds the specified information
 *
 * If there is a file in replaceFile, changes opType to replacement request
 */
ImageProt construct_request(FileStruct* files)
{
    ImageProt imageProt = {0};
    imageProt.prefix = PREFIX;

    read_stream(files->detectFile, &imageProt.img1Size, &imageProt.img1);

    if (files->replaceFile) {
        imageProt.opType = REPLACEMENT_REQUEST;
        read_stream(files->replaceFile, &imageProt.img2Size, &imageProt.img2);
    } else {
        imageProt.opType = DETECTION_REQUEST;
    }

    // Read the replacement image file here

    return imageProt;
}

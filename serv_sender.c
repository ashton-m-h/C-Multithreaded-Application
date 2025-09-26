#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "image_protocol.h"

#define PREFIX 0x23107231
#define OUTPUT 2

void read_stream(FILE* stream, uint32_t* size, uint8_t** data);

/**
 * Function that construct and returns an ImageProt,
 * assumes the temporary file /tmp/imagefile.jpg or whatever
 * it is has the output file
 *
 */
ImageProt construct_imageprot_serv(void)
{
    ImageProt imageProt = {0};
    imageProt.prefix = PREFIX;
    imageProt.opType = OUTPUT;
    uint32_t imgSize = 0;
    uint8_t* img;

    // Open file stream to the temp file
    FILE* tmpFile = fopen("/tmp/imagefile.jpg", "r");

    // Read from tmp file, and then assign
    read_stream(tmpFile, &imgSize, &img);
    imageProt.img1Size = imgSize;
    imageProt.img1 = img;

    fflush(tmpFile);
    fclose(tmpFile);
    return imageProt;
}

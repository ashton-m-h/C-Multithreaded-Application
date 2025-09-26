#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "image_protocol.h"

#define BUFF_SIZE 256
#define REPLACEMENT_REQUEST 1

/**
 * Helper function for receiving data from stream and then
 * putting the byte size into size and the data into data
 * This is for when we DONT know the image size
 *
 * stream: reading data coming from this stream
 * size: fills this uint32_t pointer with the amount of bytes that
 * comes from stream
 * data: fills this uint8_t pointer to pointer with a malloced data
 * (needs to be freed)
 */
void read_stream(FILE* stream, uint32_t* size, uint8_t** data)
{
    int imageSize = 1; // This is how many buff size multiples
    uint32_t imageByteSize = 0; // This is how many bytes
    uint8_t* imageBuffer = malloc(BUFF_SIZE * sizeof(uint8_t));
    int bytesRead = 0;
    while ((bytesRead = fread(((BUFF_SIZE) * (imageSize - 1)) + imageBuffer,
                    sizeof(uint8_t), BUFF_SIZE, stream))
            > 0) {
        // Read into image1Buffer at the correct point
        imageBuffer = realloc(
                imageBuffer, (++imageSize) * BUFF_SIZE * sizeof(uint8_t));
        imageByteSize += bytesRead;
    }

    *size = imageByteSize;
    *data = imageBuffer;
    return;
}

/**
 * sends the corresponding data out to FILE* to
 * ImageProt imageProt: Contains all image data and other
 * data needed to send conforming to the specification
 * FILE* to: FILE* that should be sending to the server
 * Writes in the order according to the spec
 */
void write_stream(ImageProt imageProt, FILE* to)
{
    // Write the prefix
    fwrite(&(imageProt.prefix), sizeof(uint32_t), 1, to);
    // Write the opType
    fwrite(&(imageProt.opType), sizeof(uint8_t), 1, to);
    // Write the img1 Size
    fwrite(&(imageProt.img1Size), sizeof(uint32_t), 1, to);
    // Write image 1
    fwrite(imageProt.img1, sizeof(uint8_t), (int)imageProt.img1Size, to);
    // Free the image 1 data
    free(imageProt.img1);
    // If replacement request write the replacement image
    if (imageProt.opType == REPLACEMENT_REQUEST) {
        fwrite(&(imageProt.img2Size), sizeof(uint32_t), 1, to);
        fwrite(imageProt.img2, sizeof(uint8_t), (int)imageProt.img2Size, to);
        free(imageProt.img2);
    }

    fflush(to);
    fclose(to);
    return;
}

/** Same as write_stream above, except ODES NOT clopse the file stream
 * Same arguments as above
 */
void write_stream_nc(ImageProt imageProt, FILE* to)
{
    int dupFd = dup(fileno(to)); // duplicate
    FILE* tempStream = fdopen(dupFd, "w");
    write_stream(imageProt, tempStream); // Closes tempStream
}

/**
 * This function is for reading the image from the protocol
 * specifically an image size and image data layers
 *
 * This assumes we already know the dataSize
 * stream: reading data from stream
 * data: reads into this
 * dataSize: fills in with the size of the data
 *
 * returns 0 for a successful read, and 1 on failure
 */
int receive_image(FILE* stream, uint8_t** data, int* dataSize)
{
    uint32_t size;
    int readBytes = 0;

    readBytes += fread(&size, sizeof(uint32_t), 1, stream);
    *dataSize = (int)size;
    *data = malloc(sizeof(uint8_t) * size);

    readBytes += fread(*data, sizeof(uint8_t), size, stream);
    if (readBytes != 1 + (int)size) {
        return 1;
    }
    return 0;
}

/**
 * This is a struct holding the fields with the relevant details
 * needed for the image processing communication protocol as
 * defined in the specification. Needs a function that can
 * process this information and then construct into one int and send over
 * if opType is set
 *
 * Stack from top to bottom:
 *
 * prefix: should be exactly as in spec
 * opType: Defines if we are sending a type of request or error message or
 * output image
 * img1Size: Amount of bytes for img1
 * img1: Pointer to image 1 data
 * img2Size: Amount of bytes for image 2
 * img2: Pointer to image 2 data
 *
 * img1 and img2 must be on the heap
 *
 */

#ifndef IMGCOMMPROTOCOL_H
#define IMGCOMMPROTOCOL_H

#include <stdint.h>

typedef struct {
    uint32_t prefix; // specific value that should be set
    uint8_t opType; // defines what happens with the images
    uint32_t img1Size; // Amount of bytes for image 1
    uint8_t* img1; // Image 1 data
    uint32_t img2Size; // Amount of bytes for image 2
    uint8_t* img2; // Image 2 data
} ImageProt;

#endif

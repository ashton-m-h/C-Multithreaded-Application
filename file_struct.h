/**
 * Struct for holding the 3 files that the user may
 * specify when using uqfaceclient.
 * detect and output are not optional but will be
 * set to stdin and stdout respectively if not specified
 * by the user
 * replaceFile: FILE* for replacing files with
 * detectFile: FILE* for the file to be processed by uqfacedetect
 * outputFile: FILE* that will be written with the output
 */

#ifndef FILESTRUCT_H
#define FILESTRUCT_H

#include <stdio.h>

typedef struct {
    FILE* replaceFile;
    FILE* detectFile;
    FILE* outputFile;
} FileStruct;

#endif

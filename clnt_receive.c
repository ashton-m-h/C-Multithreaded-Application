#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define PREFIX 0x23107231
#define COMM_ERR 16
#define SERV_ERR 19
#define OUTPUT_IMAGE 2
#define ERR_MSG 3

int receive_image(FILE* stream, uint8_t** data, int* dataSize);

/**
 * Client function that receives incoming data from the
 * file stream in and handles appropriately. Puts the
 * resulting image data into output, or fills errArg
 * with the appropriate error message
 * Returns 0 on success, else returns error code
 *
 * Closes the input file stream as well
 *
 * First checks if the prefix is correct, otherwise exits,
 * and declares a variable 'runningTotal' that checks we
 * get the correct amount of data at each layer of the
 * protocol
 */
int receive_clnt(FILE* in, char** errArg, FILE* output)
{
    int runningTotal = 0;

    uint32_t recPrefix;
    runningTotal += fread(&recPrefix, sizeof(uint32_t), 1, in);
    // Check if the received prefix is correct and
    // we have received correct number of bytes so far
    if (runningTotal != 1 || recPrefix != PREFIX) {
        fclose(in);
        return COMM_ERR;
    }

    uint8_t recOpType;
    runningTotal += fread(&recOpType, sizeof(uint8_t), 1, in);
    if (runningTotal != 2) {
        fclose(in);
        return COMM_ERR;
    }
    // Case for error message:
    if (recOpType == ERR_MSG) {
        uint8_t* errData = NULL; // Initialise to NULL
        int dataSize = 0;
        if (receive_image(in, &errData, &dataSize)) {
            return COMM_ERR;
        }
        // Realloc errData to char* so we can add null terminator
        *errArg = realloc(errData, sizeof(char) * dataSize + 1);
        (*errArg)[dataSize] = '\0';
        fclose(in);
        // No need to free as this is done in error function
        return SERV_ERR;
    }
    if (recOpType == OUTPUT_IMAGE) {
        uint8_t* data = NULL; // Initialise to NULL
        int dataSize = 0;
        if (receive_image(in, &data, &dataSize)) {
            return COMM_ERR;
        }

        // Write the data, flush and free data
        fwrite(data, sizeof(uint8_t), dataSize, output);
        fflush(output);
        fclose(in);
        free(data);
    } else { // OpType is not recognised
        fclose(in);
        return COMM_ERR;
    }

    return 0;
}

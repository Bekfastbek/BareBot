#include <stdio.h>
#include <stdlib.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include "../../include/utils.h"



void keygen(char *output) {
    unsigned char random_data[16];
    if (RAND_bytes(random_data, 16) != 1) {
        fprintf(stderr, "[ERROR] Randomization failed. Exiting...");
        exit(1);
    }

    EVP_EncodeBlock((unsigned char *)output, random_data, 16);
}
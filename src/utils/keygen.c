#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/err.h>
#include "../../include/utils.h"



static void randBytes(unsigned char *buf, const int length) {
    const int result = RAND_bytes(buf, length);
    if (result != 1) {
        fprintf(stderr, "RAND_bytes failed therefore program is exiting for security...");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}



void encodeb64(const unsigned char *raw_data, const int raw_len, char *output) {
    BIO *b64Filter = BIO_new(BIO_f_base64());
    BIO *memSink = BIO_new(BIO_s_mem());
    BIO_push(b64Filter, memSink);
    BIO_set_flags(b64Filter, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(b64Filter, raw_data, raw_len);
    BIO_flush(b64Filter);

    BUF_MEM *bptr;
    BIO_get_mem_ptr(b64Filter, &bptr);

    if (bptr -> length > 0) {
        memcpy(output, bptr -> data, bptr -> length);
        output[bptr -> length] = '\0';
    } else {
        output[0] = '\0';
    }

    BIO_free_all(b64Filter);
}



void keygen(char *output) {
    constexpr int ENTROPY_SIZE = 16;
    unsigned char rand_data[ENTROPY_SIZE];
    randBytes(rand_data, ENTROPY_SIZE);
    encodeb64(rand_data, ENTROPY_SIZE, output);
}
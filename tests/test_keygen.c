#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../include/utils.h"
#include "../include/tests.h"



bool is_base64(const char *buf) {
    for (; *buf; buf++) {
        if (!isalnum(*buf) && *buf != '+' && *buf != '/' && *buf != '=') {
            return false;
        }
    }
    return true;
}



void test_keygen() {
    printf("\n==================================\n");
    printf("[TEST] Keygen Generation\n");
    printf("==================================\n");

    char key1[32] = {0};
    char key2[32] = {0};
    bool keygen_success = true;

    keygen(key1);

    if (strlen(key1) == 24) {
        printf("[PASS] Key length is correct (24 char)\n");
    } else {
        printf("[FAIL] Key length is wrong. Got %lu, expected 24.\n", strlen(key1));
        keygen_success = false;
    }

    if (is_base64(key1)) {
        printf("[PASS] Key contains valid B64 char.\n");
    } else {
        printf("[FAIL] Key contains invalid char.\n");
        keygen_success = false;
    }

    keygen(key2);
    if (strcmp(key1, key2) != 0) {
        printf("[PASS] Both keys are random.\n");
    } else {
        printf("[FAIL] Keys are the same. Randomize logic might be broken.");
        keygen_success = false;
    }

    if (keygen_success == true) {
        printf("[INFO] Sample Keys: \n%s\n%s\n", key1, key2);
        printf("[PASS] Keygen test successful.");
        report.keygen = true;
    } else {
        printf("[FAIL] Keygen test failed.");
        report.keygen = false;
    }
}
#include <stdio.h>
#include <stdbool.h>
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

    keygen(key1);

    if (strlen(key1) == 24) {
        printf("[PASS] Key length is correct (24 char)\n");
    } else {
        printf("[FAIL] Key length is wrong. Got %lu, expected 24.\n", strlen(key1));
        report.keygen = false;
        return;
    }

    if (is_base64(key1)) {
        printf("[PASS] Key contains valid B64 char.\n");
    } else {
        printf("[FAIL] Key contains invalid char.\n");
        report.keygen = false;
        return;
    }

    keygen(key2);
    if (strcmp(key1, key2) != 0) {
        printf("[PASS] Both keys are random. Test Passed.\n");
    } else {
        printf("[FAIL] Keys are identical. Randomize logic might be broken.");
        report.keygen = false;
        return;
    }

    printf("[INFO] Sample Keys: \n%s\n%s\n", key1, key2);
    report.keygen = true;
}
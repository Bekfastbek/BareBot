#include <stdio.h>
#include "../include/tests.h"



struct Report report = {0};
int main() {
    printf("==============RUNNING TESTS==============\n\n");
    test_env();
    test_keygen();
    test_client();

    printf("\n=======================================\n");
    printf("SUMMARY:\n");
    printf("Env: %s\n", report.env ? "PASS" : "FAIL");
    printf("KeyGen: %s\n", report.keygen ? "PASS" : "FAIL");
    printf("Handshake: %s\n", report.client ? "PASS" : "FAIL");
    printf("=========================================\n");
    return 0;
}
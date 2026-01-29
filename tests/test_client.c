#include <stdio.h>
#include "../include/network.h"
#include "../include/tests.h"



void test_client() {
    printf("\n==================================\n");
    printf("[TEST] SSL Connection Handshake\n");
    printf("==================================\n");

    Client client = {0};
    bool client_success = true;

    printf("[INFO] Connecting to discord.com...\n");
    if (client_connect(&client, "discord.com", "443") == 0) {
        if (client.is_connected && client.ssl != NULL) {
            printf("[PASS] Socket is open and SSL is active.\n");
            printf("[INFO] Cipher used: %s\n", SSL_get_cipher(client.ssl));
        } else {
            printf("[FAIL] Internal state flags are wrong.\n");
            client_success = false;
        }
    } else {
        printf("[FAIL] Connection failed.\n");
        client_success = false;
    }

    client_cleanup(&client);

    if (client_success == true) {
        printf("[PASS] Client test successful.\n");
        report.client = true;
    } else {
        printf("[FAIL] Client test failed.");
        report.client = false;
    }
}
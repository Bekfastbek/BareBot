#include <stdio.h>
#include "../include/client.h"
#include "../include/tests.h"



void test_client() {
    printf("\n==================================\n");
    printf("[TEST] SSL Connection Handshake\n");
    printf("==================================\n");

    Client client = {0};
    printf("[INFO] Connecting to discord.com...\n");
    if (client_connect(&client, "discord.com", "443") != 0) {
        printf("[FAIL] Connection failed.\n");
        report.client = false;
        return;
    }

    if (client.is_connected && client.ssl != NULL) {
        printf("[PASS] Socket is open and SSL is active.\n");
        printf("[INFO] Cipher used: %s\n", SSL_get_cipher(client.ssl));
    } else {
        printf("[FAIL] Internal state flags are wrong.\n");
        client_cleanup(&client);
        report.client = false;
        return;
    }

    client_cleanup(&client);
    printf("[PASS] Cleanup completed successfully.\n");
    report.client = true;
}
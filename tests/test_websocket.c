#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../include/network.h"
#include "../include/tests.h"



void test_websocket() {
    printf("\n==================================\n");
    printf("[TEST] Websocket and Heartbeat\n");
    printf("==================================\n");

    client client = {0};
    bool ws_success = false;
    bool heartbeat_success = false;
    bool close_success = false;
    char buf[4096];

if (client_connect(&client, "gateway.discord.gg", "443") == 0) {
        printf("[1/3] Connection established. Upgrading...\n");

        if (client_upgrade(&client, "/?v=10&encoding=json") > 0) {
            memset(buf, 0, sizeof(buf));
            client_receive(&client, buf, sizeof(buf) - 1);

            if (strstr(buf, "101 Switching Protocols")) {
                printf("[PASS] Connected to Discord Gateway (101 Switching Protocols).\n");
                ws_success = true;
            } else {
                printf("[FAIL] Server did not upgrade. Response: \n%s\n", buf);
            }
        }
    } else {
        printf("[FAIL] Could not connect to gateway.discord.gg\n");
    }

    if (ws_success) {
        printf("[2/3] Starting Heartbeat Thread (Hijacked to 500ms)...\n");
        const int original_interval = heartbeat_interval;
        heartbeat_interval = 500;
        ws_start_heartbeat(&client);

        printf("[INFO] Saving stream to data/debug.bin...\n");
        FILE *fp = fopen("../data/debug.bin", "wb");
        if (!fp) {
            printf("[FAIL] Could not open file.\n");
        }

        int total = 0;
        for (int i = 0; i < 25; i++) {
            const int len = client_receive(&client, buf, sizeof(buf));
            if (len > 0) {
                if (strstr(buf, "\"op\":11")) {
                    printf("[SUCCESS] Received Heartbeat from Discord\n");
                    heartbeat_success = true;
                }
                if (fp) {
                    fwrite(buf, 1, len, fp);
                    fflush(fp);
                }
                total += len;
                printf("[INFO] Read %d bytes\n", len);
            } else if (len == 0) {
                printf("[INFO] Server closed connection.\n");
                break;
            } else {
                usleep(100000);
            }
        }
        if (fp) {
            fclose(fp);
        }

        printf("[3/3] Stopping Heartbeat Thread...\n");
        ws_stop();

        if (loop == false) {
            printf("[PASS] Stop signal sent successfully.\n");
            close_success = true;
        } else {
            printf("[FAIL] Stop signal failed.\n");
        }

        heartbeat_interval = original_interval;
        usleep(200000);
    }
    client_cleanup(&client);

    if (ws_success == true && heartbeat_success == true && close_success == true) {
        printf("[PASS] Websocket Test Successful.");
        report.websocket = true;
    } else {
        printf(
            "[FAIL] Websocket Test Failed.\nConnection = %s\nHeartbeat= %s\nClose = %s\n",
                ws_success ? "Success" : "Failed",
                heartbeat_success ? "Success" : "Failed",
                close_success ? "Success" : "Failed"
                );
        report.websocket = false;
    }
}
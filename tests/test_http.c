#include <stdio.h>
#include <string.h>
#include "../include/utils.h"
#include "../include/network.h"
#include "../include/tests.h"



void test_http() {
    printf("\n==================================\n");
    printf("[TEST] HTTP Protocols (POST & GET) \n");
    printf("==================================\n");

    Client client = {0};
    char buf[4096];
    bool get_success = false;
    bool post_success = false;

    if (client_connect(&client, "discord.com", "443") == 0) {
        printf("[1/2] Testing GET /users/@me...\n");
        client_get(&client, "/api/users/@me");

        memset(buf, 0, sizeof(buf));
        client_receive(&client, buf, sizeof(buf) -1);

        if (strstr(buf, "200 OK")) {
            printf("[PASS] GET Successful (200 OK).\n");
            get_success = true;
        } else {
            printf("[FAIL] GET Failed. Response: \n%s\n", buf);
        }
    } else {
            printf("[FAIL] Connection Failed (Network Error).\n");
    }


    client_cleanup(&client);
    memset(&client, 0, sizeof(client));

    if (client_connect(&client, "discord.com", "443") == 0) {
        printf("[2/2] Testing POST message...\n");
        char path[128];
        snprintf(path, sizeof(path), "/api/v10/channels/%s/messages", config.channelid);
        const char *json = "{\"content\": \"BareBot Test Success\"}";

        client_post(&client, path, json);
        memset(buf, 0, sizeof(buf));
        client_receive(&client, buf, sizeof(buf) - 1);

        if (strstr(buf, "200 OK") || strstr(buf, "201 Created")) {
            printf("[PASS] POST Successful (Message Sent).\n");
            post_success = true;
        } else {
            printf("[FAIL] POST Failed. Response: \n%s\n", buf);
        }
    } else {
        printf("[FAIL] Re-connection for POST Failed.\n");
    }

    client_cleanup(&client);
    memset(&client, 0, sizeof(client));



    if (get_success == true && post_success == true) {
        printf("[PASS] HTTP Test Successful.");
        report.http = true;
    } else {
        printf(
            "[FAIL] HTTP Test Failed.\nGET = %s\nPOST= %s\n",
                get_success ? "Success" : "Failed",
                post_success ? "Success" : "Failed"
                );
        report.http = false;
    }
}
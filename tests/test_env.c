#include <stdio.h>
#include <string.h>
#include "../include/env.h"
#include "../include/tests.h"



void test_env() {
    printf("\n==================================\n");
    printf("[TEST] Env Loader\n");
    printf("==================================\n");

    memset(&config, 0, sizeof(config));
    loadenv();
    bool env_success = true;

    if (config.discord_token != NULL && strlen(config.discord_token) > 0) {
        printf("[PASS] Token loaded.\n");
    } else {
        printf("[FAIL] Token is NULL or empty.\n");
        env_success = false;
    }
    if (config.channelid != NULL && strlen(config.channelid) > 0) {
        printf("[PASS] Channel ID loaded: %s\n", config.channelid);
    } else {
        printf("[FAIL] Channel ID is NULL or empty.\n");
        env_success = false;
    }

    if (env_success == true) {
        printf("[PASS] Env Parsing is successful.");
        report.env = true;
    } else {
        printf("[FAIL] Env Parsing failed.");
        report.env =  false;
    }
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../../include/utils.h"



struct config_env config;



char *remove_whitespace(char *buf) {
    while (isspace(*buf)) {
        buf++;
    }
    if (*buf == 0) {
        return buf;
    }

    char *end = buf + strlen(buf) - 1;
    while (end > buf && isspace(*end)) {
        end--;
    }
    end[1] = '\0';
    return buf;
}



void loadenv() {
    FILE *buf = fopen("../.env", "r");

    if (buf == NULL) {
        printf("File empty or improperly set. Exiting...");
        exit(1);
    }

    fseek(buf, 0, SEEK_END);
    const long length = ftell(buf);
    rewind(buf);
    char *stream = malloc(length + 1);

    if (stream == NULL) {
        printf("Malloc failed. Exiting...");
        exit(1);
    }

    while (fgets(stream, (int)length+1, buf)) {
        stream[strcspn(stream, "\n")] = 0;
        char *key = strtok(stream, "=");
        if (key == NULL) {
            continue;
        }
        char *value = strtok(nullptr, "");
        if (value == NULL) {
            continue;
        }

        key = remove_whitespace(key);
        value = remove_whitespace(value);

        if (strcmp(key, "DISCORD_TOKEN") == 0 || strcmp(key, "DISCORD_TOKEN ") == 0) {
            const unsigned short token_len = strlen(value);
            config.discord_token = malloc(token_len + 1);
            strcpy(config.discord_token, value);
        }
        if (strcmp(key, "CHANNEL_ID") == 0 || strcmp(key, "CHANNEL_ID ") == 0) {
            const unsigned short id_len = strlen(value);
            config.channelid = malloc(id_len + 1);
            strcpy(config.channelid, value);
        }
    }
    fclose(buf);
    free(stream);

    if(config.discord_token == NULL || config.channelid == NULL) {
        printf("Environment variables not set properly. Exiting...");
        exit(1);
    }
}
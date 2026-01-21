#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../include/env.h"



char *remove_whitespace(char *buf) {
    char *end;
    while (isspace(*buf)) {
        buf++;
    }
    if (*buf == 0) {
        return buf;
    }
    end = buf + strlen(buf) - 1;
    while (end > buf && isspace(*end)) {
        end--;
    }
    end[1] = '\0';
    return buf;
}



struct Config config;
void loadenv() {
    FILE *buf;
    buf = fopen("../.env", "r");

    if (buf == NULL) {
        printf("File empty or improperly set. Exiting...");
        exit(1);
    }

    fseek(buf, 0, SEEK_END);
    long long length = ftell(buf);
    rewind(buf);
    char *stream = malloc(length + 1);

    if (stream == NULL) {
        printf("Malloc failed. Exiting...");
        exit(1);
    }

    while (fgets(stream, length+1, buf)) {
        stream[strcspn(stream, "\n")] = 0;
        char *key = strtok(stream, "=");
        char *value = strtok(NULL, "");

        if (key == NULL || value == NULL) {
            continue;
        }

        key = remove_whitespace(key);
        value = remove_whitespace(value);

        if (strcmp(key, "DISCORD_TOKEN") == 0 || strcmp(key, "DISCORD_TOKEN ") == 0) {
            size_t token_len = strlen(value);
            config.discord_token = malloc(token_len + 1);
            strcpy(config.discord_token, value);
        }
        if (strcmp(key, "CHANNEL_ID") == 0 || strcmp(key, "CHANNEL_ID ") == 0) {
            size_t id_len = strlen(value);
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
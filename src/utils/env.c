#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../../include/utils.h"



struct config_env config;



char *remove_whitespace(char *buffer) {
    while (isspace(*buffer)) {
        buffer++;
    }
    if (*buffer == 0) {
        return buffer;
    }

    char *end = buffer + strlen(buffer) - 1;
    while (end > buffer && isspace(*end)) {
        end--;
    }
    end[1] = '\0';
    return buffer;
}



void loadenv() {
    FILE *buffer = fopen("../.env", "r");

    if (buffer == NULL) {
        printf("File empty or improperly set. Exiting...");
        exit(1);
    }

    fseek(buffer, 0, SEEK_END);
    const long length = ftell(buffer);
    rewind(buffer);
    char *stream = malloc(length + 1);

    if (stream == NULL) {
        printf("Malloc failed. Exiting...");
        exit(1);
    }

    while (fgets(stream, (int)length+1, buffer)) {
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
            const unsigned short token_length = strlen(value);
            config.discord_token = malloc(token_length + 1);
            strcpy(config.discord_token, value);
        }
        if (strcmp(key, "CHANNEL_ID") == 0 || strcmp(key, "CHANNEL_ID ") == 0) {
            const unsigned short id_length = strlen(value);
            config.channelid = malloc(id_length + 1);
            strcpy(config.channelid, value);
        }
    }
    fclose(buffer);
    free(stream);

    if(config.discord_token == NULL || config.channelid == NULL) {
        printf("Environment variables not set properly. Exiting...");
        exit(1);
    }
}
#include <stdio.h>
#include <string.h>
#include "../../../include/utils.h"
#include "../../../include/network.h"



int client_post(const client *client, const char *path, const char *json_body) {
    if (!client -> is_connected || !client -> ssl) {
        return -1;
    }

    char request_buffer[1024];
    const unsigned short body_length = (unsigned int)strlen(json_body);

    const int length = snprintf(request_buffer, sizeof(request_buffer),
        "POST %s HTTP/1.1\r\n"
        "Host: discord.com\r\n"
        "Authorization: Bot %s\r\n"
        "User-Agent: DiscordBot(BareBot, 1.0)\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %u\r\n"
        "\r\n"
        "%s",
        path,
        config.discord_token,
        body_length,
        json_body
        );

    if (length < 0 || (unsigned int)length >= sizeof(request_buffer)) {
        fprintf(stderr, "[ERROR] Requested Buffer is too large");
        return -1;
    }
    if (client_send(client, request_buffer, length) < 0) {
        return -1;
    }
    printf("[INFO] Sending POST to %s\n", path);
    return client_send(client, json_body, length);
}


int client_get(const client *client, const char *path) {
    if (!client -> is_connected || !client -> ssl) {
        return -1;
    }

    char request_buffer[512];

    const int length = snprintf(request_buffer, sizeof(request_buffer),
        "GET %s HTTP/1.1\r\n"
        "Host: discord.com\r\n"
        "Authorization: Bot %s\r\n"
        "User-Agent: DiscordBot(BareBot, 1.0)\r\n"
        "Connection: close\r\n"
        "\r\n",
        path,
        config.discord_token);
    if (length < 0 || length >= sizeof(request_buffer)) {
        fprintf(stderr, "[ERROR] GET request too large.\n");
        return -1;
    }

    printf("[INFO] Sending info to %s\n", path);
    return client_send(client, request_buffer, length);
}
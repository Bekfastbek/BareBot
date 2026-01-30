#include <stdio.h>
#include <string.h>
#include "../../../include/utils.h"
#include "../../../include/network.h"



int client_post(const client *client, const char *path, const char *json_body) {
    if (!client -> is_connected || !client -> ssl) {
        return -1;
    }

    char headers[1024];
    const unsigned short body_len = (unsigned int)strlen(json_body);

    const int len = snprintf(headers, sizeof(headers),
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
        body_len,
        json_body
        );

    if (len < 0 || (unsigned int)len >= sizeof(headers)) {
        fprintf(stderr, "[ERROR] Requested Buffer is too large");
        return -1;
    }
    if (client_send(client, headers, len) < 0) {
        return -1;
    }
    printf("[INFO] Sending POST to %s\n", path);
    return client_send(client, json_body, len);
}


int client_get(const client *client, const char *path) {
    if (!client -> is_connected || !client -> ssl) {
        return -1;
    }

    char req_buf[512];

    const int len = snprintf(req_buf, sizeof(req_buf),
        "GET %s HTTP/1.1\r\n"
        "Host: discord.com\r\n"
        "Authorization: Bot %s\r\n"
        "User-Agent: DiscordBot(BareBot, 1.0)\r\n"
        "Connection: close\r\n"
        "\r\n",
        path,
        config.discord_token);
    if (len < 0 || len >= sizeof(req_buf)) {
        fprintf(stderr, "[ERROR] GET request too large.\n");
        return -1;
    }

    printf("[INFO] Sending info to %s\n", path);
    return client_send(client, req_buf, len);
}
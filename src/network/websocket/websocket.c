#include <stdio.h>
#include "../../../include/utils.h"
#include "../../../include/network.h"



int client_upgrade(const client *client, const char *path) {
    if (!client -> is_connected || !client -> ssl) {
        return -1;
    }

    char request_buffer[512];
    char key[32];
    keygen(key);

    const int length = snprintf(request_buffer, sizeof(request_buffer),
    "GET %s HTTP/1.1\r\n"
        "Host: gateway.discord.gg\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: %s\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n",
        path,
        key
    );

    if (length < 0 || length >= sizeof(request_buffer)) {
        fprintf(stderr, "[ERROR] GET request too large.\n");
        return -1;
    }
    printf("[INFO] Sending upgrade signal to discord...\n");
    return client_send(client, request_buffer, length);
}



void ws_stop() {
    __atomic_store_n(&loop, false, __ATOMIC_SEQ_CST);
}
#include <stdio.h>
#include "../../../include/utils.h"
#include "../../../include/network.h"



int client_upgrade(const client *client, const char *path) {
    if (!client -> is_connected || !client -> ssl) {
        return -1;
    }

    char req_buf[512];
    char key[32];
    keygen(key);

    const int len = snprintf(req_buf, sizeof(req_buf),
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

    if (len < 0 || len >= sizeof(req_buf)) {
        fprintf(stderr, "[ERROR] GET request too large.\n");
        return -1;
    }
    printf("[INFO] Sending upgrade signal to discord...\n");
    return client_send(client, req_buf, len);
}



void ws_stop() {
    __atomic_store_n(&loop, false, __ATOMIC_SEQ_CST);
}
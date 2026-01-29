#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <openssl/rand.h>
#include "../../include/utils.h"
#include "../../include/network.h"



int last_sequence = 0;
bool has_received_seq = false;
int heartbeat_interval = 41250;
volatile bool loop = true;



int client_upgrade(Client *client, const char *path) {
    if (!client -> is_connected || !client -> ssl) {
        return -1;
    }

    char req_buf[4096];
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
    return client_send(client, req_buf, (unsigned int)len);
}



int parse_header(const unsigned char *data, uint64_t *out_len) {
    const unsigned char len_indicator =data[1] & 0x7F;

    if (len_indicator < 126) {
        *out_len =len_indicator;
        return 2;
    } else if (len_indicator == 126) {
        *out_len = (data[2] << 8) |data[3];
        return 4;
    } else {
        uint64_t len = 0;
        for (int i = 0; i < 8; i++) {
            len = (len << 8) |data[2 + i];
        }
        *out_len = len;
        return 10;
    }
}



void generate_mask(unsigned char *mask_key) {
    if (RAND_bytes(mask_key, 4) != 1) {
        mask_key[0] = 0xAA;
        mask_key[1] = 0xBB;
        mask_key[2] = 0xCC;
        mask_key[3] = 0xDD;
    }
}



int ws_send_text(Client *client, const char *text) {
    const unsigned int len = strlen(text);
    unsigned char *frame = malloc(len + 14);
    if (!frame) {
        fprintf(stderr, "[ERROR] Failed to allocate memory in ws_send_text\n");
        return -1;
    }
    int idx = 0;
    frame[idx++] = 0x81;

    if (len <= 125) {
        frame[idx++] = 0x80 | len;
    } else if (len <= 65535) {
        frame[idx++] = 0x80 | 126;
        frame[idx++] = (len >> 8) & 0xFF;
        frame[idx++] = len & 0xFF;
    } else {
        frame[idx++] = 0x80 | 127;
        for (int i = 7; i >= 0; i--) {
            frame[idx++] = (len >> (i * 8)) & 0xFF;
        }
    }

    unsigned char mask_key[4];
    generate_mask(mask_key);
    memcpy(&frame[idx], mask_key, 4);
    idx += 4;

    for (int i = 0; i < len; i++) {
        frame[idx + i] = text[i] ^ mask_key[i % 4];
    }

    const int result = client_send(client, (char *)frame, idx + len);
    free(frame);
    return result;
}



void send_heartbeat(Client *client) {
    char payload[64];

    if (has_received_seq) {
        sprintf(payload, "{\"op\":1,\"d\":%d}", last_sequence);
    } else {
        sprintf(payload, "{\"op\":1,\"d\":null}");
    }

    ws_send_text(client, payload);
    printf("[HEARTBEAT] Sent %s\n", payload);
}



void ws_stop() {
    loop = false;
}



void *ws_heartbeat_thread(void *arg) {
    Client *client = (Client *)arg;
    printf("[THREAD0] Heartbeat thread started.\n");

    while (loop) {
        usleep(heartbeat_interval * 1000);
        send_heartbeat(client);
    }
    return NULL;
}



void ws_start_heartbeat(Client *client) {
    pthread_t thread_id;

    if (pthread_create(&thread_id, nullptr, ws_heartbeat_thread, client) != 0) {
        fprintf(stderr, "[ERROR] Failed to start heartbeat thread.\n");
    } else {
        pthread_detach(thread_id);
    }
}
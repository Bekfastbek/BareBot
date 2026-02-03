#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <openssl/rand.h>
#include "../../../include/network.h"



pthread_mutex_t ws_lock = PTHREAD_MUTEX_INITIALIZER;
volatile int heartbeat_interval = 41250 * 1000;
constexpr int heartbeat_size = 128;
volatile int last_sequence = 0;
volatile bool has_received_sequence = false;
volatile bool loop = true;



int ws_send_text(const client *client, const char *text) {
    const unsigned int length = strlen(text);
    if (length > 100) {
        return 1;
    }

    unsigned char frame[heartbeat_size];
    int index = 0;

    frame[index++] = 0x81;
    frame[index++] = 0x80 | length;

    unsigned char mask_key[4];
    if (RAND_bytes(mask_key, 4) != 1) {
        mask_key[0] = 0xAA;
        mask_key[1] = 0xBB;
        mask_key[2] = 0xCC;
        mask_key[3] = 0xDD;
    }
    memcpy(&frame[index], mask_key, 4);
    index += 4;

    for (int i = 0; i < length; i++) {
        frame[index + i] = text[i] ^ mask_key[i % 4];
    }

    pthread_mutex_lock(&ws_lock);
    const int sent = client_send(client, (const char*)frame, (int)(index + length));
    pthread_mutex_unlock(&ws_lock);
    return sent;
}



void send_heartbeat(const client *client) {
    char payload[64];

    if (__atomic_load_n(&has_received_sequence, __ATOMIC_SEQ_CST)) {
        snprintf(payload, sizeof(payload), "{\"op\":1,\"d\":%d}", __atomic_load_n(&last_sequence, __ATOMIC_SEQ_CST));
    } else {
        snprintf(payload, sizeof(payload), "{\"op\":1,\"d\":null}");
    }

    ws_send_text(client, payload);
    printf("[HEARTBEAT] Sent %s\n", payload);
}



void *ws_heartbeat_thread(void *arguments) { // Don't want to make this const since pthread format gets messed up
    const client *client = arguments;
    printf("[THREAD] Heartbeat thread started.\n");

    while (__atomic_load_n(&loop, __ATOMIC_SEQ_CST)) {
        usleep(__atomic_load_n(&heartbeat_interval, __ATOMIC_SEQ_CST));
        send_heartbeat(client);
    }
    return nullptr;
}



void ws_start_heartbeat(client *client) {
    pthread_t thread_id;

    if (pthread_create(&thread_id, nullptr, ws_heartbeat_thread, client) != 0) {
        fprintf(stderr, "[ERROR] Failed to start heartbeat thread.\n");
    } else {
        pthread_detach(thread_id);
    }
}
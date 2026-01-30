#ifndef CLIENT_H
#define CLIENT_H
#include <openssl/ssl.h>



extern volatile int last_sequence;
extern volatile bool has_received_seq;
extern volatile int heartbeat_interval;
extern volatile bool loop;

typedef struct {
    int socket_file_descriptor;
    SSL_CTX *context;
    SSL *ssl;
    bool is_connected;
} client;

int client_connect(client *client, const char *hostname, const char *port);
int client_send(const client *client, const char *data, int len);
int client_receive(const client *client, char *buf, int len);
int client_post(const client *client, const char *path, const char *json_body);
int client_get(const client *client, const char *path);
void client_cleanup(client *client);

int client_upgrade(const client *client, const char *path);
int parse_header(const unsigned char *data, uint64_t *out_len);
int ws_send_text(const client *client, const char *text);
void ws_start_heartbeat(client *client);
void ws_stop();



#endif
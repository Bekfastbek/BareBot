#ifndef CLIENT_H
#define CLIENT_H
#include <openssl/ssl.h>



extern int last_sequence;
extern bool has_received_seq;
extern int heartbeat_interval;
extern volatile bool loop;

typedef struct {
    int socket_file_descriptor;
    SSL_CTX *context;
    SSL *ssl;
    bool is_connected;
} Client;

int client_connect(Client *client, const char *hostname, const char *port);
int client_send(const Client *client, const char *data, int len);
int client_receive(const Client *client, char *buf, int len);
int client_post(const Client *client, const char *path, const char *json_body);
int client_get(const Client *client, const char *path);
void client_cleanup(Client *client);

int client_upgrade(const Client *client, const char *path);
int parse_header(const unsigned char *data, uint64_t *out_len);
int ws_send_text(const Client *client, const char *text);
void ws_start_heartbeat(Client *client);
void ws_stop();



#endif
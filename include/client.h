#ifndef CLIENT_H
#define CLIENT_H

#include <openssl/err.h>

typedef struct {
    int socket_file_descriptor;
    SSL_CTX *context;
    SSL *ssl;
    int is_connected;
} DiscordClient;

int client_connect(DiscordClient *client, const char *hostname, const char *port);
void client_cleanup(DiscordClient *client);

#endif